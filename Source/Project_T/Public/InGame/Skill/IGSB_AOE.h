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

	uint8 currentDuration;

	FTSTicker::FDelegateHandle durationHandle{};

public:
	AIGSB_AOE();

	void BeginPlay() override;
	void UseSkill() override;
	virtual void InitSkill();
	virtual void Tick(float _DeltaTime) override;

private:
	UFUNCTION()
	void OnStartOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);

	bool DamageoverTime(float _DeltaTime);
};
