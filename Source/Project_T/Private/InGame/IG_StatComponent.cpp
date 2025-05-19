#include "InGame/IG_StatComponent.h"
#include "Net/UnrealNetwork.h"

void UIG_StatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UIG_StatComponent, characterState);
}

void UIG_StatComponent::OnRep_StateChange()
{
	onChangeState.Broadcast(characterState);
}
