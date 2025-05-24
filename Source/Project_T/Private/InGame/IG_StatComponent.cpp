#include "InGame/IG_StatComponent.h"
#include "Net/UnrealNetwork.h"

UIG_StatComponent::UIG_StatComponent()
{
	
	SetIsReplicatedByDefault(true);
}

void UIG_StatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UIG_StatComponent, characterState);
	DOREPLIFETIME(UIG_StatComponent, characterStatus);
}

void UIG_StatComponent::SetCharacterState(E_CHARACTER_STATE _NewState)
{
	characterState = _NewState;
	onChangeState.Broadcast(characterState);
}

void UIG_StatComponent::SetCharacterStatus(F_CHARACTER_STATUS _NewStatus)
{
	characterStatus = _NewStatus;
	onChangeStatus.Broadcast(characterStatus);
}

void UIG_StatComponent::Server_AddExperience_Implementation(int32 _Exp)
{
	AddExperience(_Exp);
}

void UIG_StatComponent::AddExperience(int32 _Exp)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		// 클라 -> 서버
		Server_AddExperience(_Exp);
		return;
	}

	if (characterStatus.currentEXP < 0)
	{
		characterStatus.currentEXP = 0;
	}
	if (characterStatus.nextLevelEXP < 0)
	{
		characterStatus.nextLevelEXP = 0;
		return;
	}

	characterStatus.currentEXP += _Exp;

	if (characterStatus.currentEXP >= characterStatus.nextLevelEXP)
	{
		characterStatus.currentEXP -= characterStatus.nextLevelEXP;
		LevelUp();
	}
}

void UIG_StatComponent::LevelUp()
{
	characterStatus.level++;

	characterStatus.nextLevelEXP *= 1.5f;
	characterStatus.damage += 5.f;
	characterStatus.attackRange += 20.f;
}


void UIG_StatComponent::OnRep_StateChange()
{
	onChangeState.Broadcast(characterState);
}

void UIG_StatComponent::OnRep_StatusChange()
{
	PTT_LOG(Warning, TEXT("OnRep_StatusChange 실행"));
	onChangeStatus.Broadcast(characterStatus);
}
