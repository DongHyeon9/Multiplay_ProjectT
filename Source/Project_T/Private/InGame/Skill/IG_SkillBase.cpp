#include "InGame/Skill/IG_SkillBase.h"
#include "Net/UnrealNetwork.h"

void AIG_SkillBase::InitSkill()
{
	bUseSkill = true;
	coolDownHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_SkillBase::SkillInterval), coolDown);
}

bool AIG_SkillBase::SkillInterval(float _DeltaTime)
{
	UseSkill();

	return bUseSkill;
}

void AIG_SkillBase::SetSkillUse(bool _bIsUse)
{
	bUseSkill = _bIsUse;
}

void AIG_SkillBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_SkillBase, bUseSkill);
}
