#include "InGame/Player/IG_SkillComponent.h"

UIG_SkillComponent::UIG_SkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UIG_SkillComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UIG_SkillComponent::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

}

