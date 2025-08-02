#include "InGame/Component/Enemy/IG_EnemyStatComponent.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/Component/Player/IG_PlayerStatComponent.h"
#include "GameFramework/PlayerState.h"

namespace INGAME_ENEMY_STAT_COMP
{
	static constexpr const TCHAR* DATA_TABLE{ TEXT("/Game/07_GameData/CSV/CV_Enemy") };
}

UIG_EnemyStatComponent::UIG_EnemyStatComponent(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FObjectFinder<UDataTable> CV_PLAYER{ INGAME_ENEMY_STAT_COMP::DATA_TABLE };

	if (CV_PLAYER.Succeeded()) statDataTable = CV_PLAYER.Object;
}

void UIG_EnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();

	tableMax = statDataTable->GetRowNames().Num() - 1;
}

void UIG_EnemyStatComponent::InitStat(int32 _Idx)
{
	Super::InitStat(_Idx);
	SetCharacterState(E_CHARACTER_STATE::ENABLE);
}

void UIG_EnemyStatComponent::ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage)
{
	SERVER_ONLY_COMP();

	Super::ApplyDamage(_Instigator, _Actor, _Damage);

	if (GetCharacterState() != E_CHARACTER_STATE::DEAD) return;

	auto pc{ Cast<AIG_PlayerController>(_Instigator) }; if (!pc) return;
	auto ps{ pc->GetPlayerState<APlayerState>() }; if (!ps) return;
	auto playerPawn{ pc->GetPawn() }; if (!playerPawn) return;
	auto playerStat{ playerPawn->FindComponentByClass<UIG_PlayerStatComponent>() }; if (!playerStat) return;
	auto enemyData{ GetCharacterData<FCharacterData_Enemy>() }; if (!enemyData) return;

	// 경험치 추가
	playerStat->AddEXP(enemyData->dropEXP);
	// 점수 더하기
	ps->SetScore(ps->GetScore() + 1);
}
