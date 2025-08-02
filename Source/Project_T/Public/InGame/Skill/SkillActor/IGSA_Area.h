#pragma once

#include "InGame/Skill/SkillActor/IG_SkillActorBase.h"
#include "IGSA_Area.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UIG_StatComponent;

UCLASS()
class PROJECT_T_API AIGSA_Area : public AIG_SkillActorBase
{
	GENERATED_BODY()
public:
	static const FName ROOT_COMP_NAME;
	static const FName FX_COMP_NAME;
	static const FName COLLISION_COMP_NAME;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSA_Area", meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> rootComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSA_Area", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraComponent> fxComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSA_Area", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> collisionComp{};

	float interval{};
	float acc{};

	UPROPERTY()
	TSet<UIG_StatComponent*> overlappedStatComp{};

public:
	AIGSA_Area(const FObjectInitializer& _Init);
	void InitSkillActor(const FSkillInitializeParameter& _Param)override;
	void BeginPlay()override;
	void ActiveSkill(float _Duration=0.0f)override;
	void InactiveSkill()override;
	void InitArea(float _Interval);
	void Tick(float _DeltaTime)override;

protected:
	void OnRep_UpdateActive()override;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex);
};
