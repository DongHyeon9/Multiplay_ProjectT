#include "InGame/IG_StatComponent.h"
#include "Net/UnrealNetwork.h"

void UIG_StatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UIG_StatComponent, characterState);
}

void UIG_StatComponent::SetCharacterState(E_CHARACTER_STATE _NewState)
{
	characterState = _NewState;
	onChangeState.Broadcast(characterState);
}

void UIG_StatComponent::OnRep_StateChange()
{
	onChangeState.Broadcast(characterState);
}
