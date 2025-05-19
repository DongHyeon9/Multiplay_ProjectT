#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGSB_Spin.generated.h"

UCLASS()
class PROJECT_T_API AIGSB_Spin : public AIG_SkillBase
{
	GENERATED_BODY()
public:
	virtual void InitSkill();
	void UseSkill() override;
	
	
	
};
