#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGSB_AOE.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_T_API AIGSB_AOE : public AIG_SkillBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_AOE", meta = (AllowPrivateAccess = true))
	float radius{ 300.f };
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_AOE", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> skillMesh{};

public:
	AIGSB_AOE();

	void BeginPlay() override;
	virtual void InitSkill();
	void UseSkill() override;
	
	void DamageoverTime();
	
};
