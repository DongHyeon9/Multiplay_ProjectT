#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGSB_LineTrace.generated.h"


UCLASS()
class PROJECT_T_API AIGSB_LineTrace : public AIG_SkillBase
{
	GENERATED_BODY()
public:
	virtual void InitSkill();
	void UseSkill() override;
	
	
};
