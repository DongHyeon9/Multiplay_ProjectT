#include "InGame/Component/Player/IG_PlayerStatComponent.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/IG_GameState.h"
#include "Net/UnrealNetwork.h"

namespace INGAME_PLAYER_STAT_COMP
{
	static constexpr const TCHAR* DATA_TABLE{ TEXT("/Game/07_GameData/CSV/CV_Player") };
}

UIG_PlayerStatComponent::UIG_PlayerStatComponent(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FObjectFinder<UDataTable> CV_PLAYER{ INGAME_PLAYER_STAT_COMP::DATA_TABLE };

	if (CV_PLAYER.Succeeded()) statDataTable = CV_PLAYER.Object;
}

void UIG_PlayerStatComponent::InitStat(int32 _Idx)
{
	Super::InitStat(_Idx);
	SetLevel(_Idx);
}

void UIG_PlayerStatComponent::ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage)
{
	SERVER_ONLY_COMP();

	Super::ApplyDamage(_Instigator, _Actor, _Damage);

	if (GetCharacterState() != E_CHARACTER_STATE::DEAD) return;

	if (auto player{ GetOwner<AIGC_Player>() })
		player->GameOver();
}

void UIG_PlayerStatComponent::SetEXP(float _NewEXP)
{
	SERVER_ONLY_COMP();
	if (playerLevel >= PLAYER_MAX_LEVEL) return;
	auto data = GetCharacterData<FCharacterData_Player>();
	while (_NewEXP >= data->needEXP)
	{
		if (playerLevel >= PLAYER_MAX_LEVEL)
		{
			_NewEXP = 0.0f;
			break;
		}
		else
		{
			_NewEXP -= data->needEXP;
			LevelUp();
			data = GetCharacterData<FCharacterData_Player>();
		}
	}
	BROAD_CAST_CHANGE(onEXPChange, currentEXP, _NewEXP);
}

void UIG_PlayerStatComponent::AddEXP(float _AddEXP)
{
	SetEXP(currentEXP + _AddEXP);
}

void UIG_PlayerStatComponent::SetLevel(int32 _NewLevel)
{
	SERVER_ONLY_COMP();
	if (GetCharacterState() != E_CHARACTER_STATE::ENABLE) return;
	if (playerLevel >= PLAYER_MAX_LEVEL) return;
	auto prevData{ playerLevel };
	playerLevel = _NewLevel;
	SetDataTable(FMath::Clamp(playerLevel - 1, 0, PLAYER_MAX_LEVEL - 1));
	auto playerData = GetCharacterData<FCharacterData_Player>();
	checkf(playerData, TEXT("Is Valid Data Table"));
	if (playerLevel >= PLAYER_MAX_LEVEL)
	{
		BROAD_CAST_CHANGE(onEXPChange, currentEXP, 0.0f);
	}
	SetHP(playerData->maxHP);
	onLevelChange.Broadcast(prevData, playerLevel);
}

void UIG_PlayerStatComponent::LevelUp(int32 _AddLevel)
{
	SetLevel(FMath::Clamp(playerLevel + _AddLevel, PLAYER_MIN_LEVEL, PLAYER_MAX_LEVEL));
}

void UIG_PlayerStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIG_PlayerStatComponent, currentEXP);
	DOREPLIFETIME(UIG_PlayerStatComponent, playerLevel);
}

void UIG_PlayerStatComponent::OnRep_EXPChange(float _Prev)
{
	onEXPChange.Broadcast(_Prev, currentEXP);
}

void UIG_PlayerStatComponent::OnRep_LevelChange(int32 _Prev)
{
	onLevelChange.Broadcast(_Prev, playerLevel);
}
