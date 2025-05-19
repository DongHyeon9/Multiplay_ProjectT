#include "InGame/Skill/IGSB_AOE.h"

void AIGSB_AOE::InitSkill()
{
	skillName = TEXT("AOE");
	level = 1;
	duration = 0.5f;
	coolDown = 5.f;
	PTT_LOG(Warning, TEXT("skillName : %s\nlevel : %d\nduration : %0.1f\ncoolDown : %0.1f"), *skillName.ToString(), level, duration, coolDown);
	Super::InitSkill();
}

void AIGSB_AOE::UseSkill()
{

	PTT_LOG(Warning, TEXT("Use A.O.E"));
}
