#include "InGame/IG_StatComponent.h"

UIG_StatComponent::UIG_StatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UIG_StatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UIG_StatComponent::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

}

