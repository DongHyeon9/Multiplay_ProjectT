#include "InGame/Player/IG_SkillComponent.h"
#include "InGame/Skill/IG_SkillBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

UIG_SkillComponent::UIG_SkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UIG_SkillComponent::UseSkill(E_SKILL_TYPE SkillType)
{
	for (AIG_SkillBase* Skill : SkillInstances)
	{
		if (Skill && Skill->GetLocalRole() == ROLE_Authority && Skill->SkillInfo.Type == SkillType)
		{
			Skill->UseSkill();
		}
	}
}

void UIG_SkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UIG_SkillComponent, SkillInstances);
}

void UIG_SkillComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{

	}
}

void UIG_SkillComponent::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

}

