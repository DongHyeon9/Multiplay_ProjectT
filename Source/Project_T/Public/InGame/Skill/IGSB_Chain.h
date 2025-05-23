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
	void BeginPlay() override;
	virtual void InitSkill();
	void UseSkill() override;

};
