#include "InGame/Skill/IG_SkillBase.h"
#include <Net/UnrealNetwork.h>

void UIG_SkillBase::UseSkill()
{
}

void UIG_SkillBase::UseSkill_Implementation()
{
}

void UIG_SkillBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIG_SkillBase, skillInfos);
}
