#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGSB_Chain.generated.h"

class AIGC_Enemy;

UCLASS()
class PROJECT_T_API AIGSB_Chain : public AIG_SkillBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGSB_Chain")
	uint8 maxChain{ 2 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGSB_Chain")
	float chainDistance{ 500.f };

public:
	virtual void InitSkill();
	void UseSkill() override;
	
	AIGC_Enemy* FindTarget(AIGC_Enemy* _Enemy,const TArray<AIGC_Enemy*>& _ExceptionEnemys);

};
