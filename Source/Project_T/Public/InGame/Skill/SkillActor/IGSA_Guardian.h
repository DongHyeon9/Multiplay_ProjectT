#pragma once

#include "InGame/Skill/SkillActor/IG_SkillActorBase.h"
#include "IGSA_Guardian.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class PROJECT_T_API AIGSA_Guardian : public AIG_SkillActorBase
{
	GENERATED_BODY()
public:
	const static FName ROOT_COMP_NAME;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSA_Guardian", meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> rootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSA_Guardian", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UStaticMeshComponent>> meshComps;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSA_Guardian", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<USphereComponent>> collisionComps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGSA_Guardian", meta = (AllowPrivateAccess = true))
	FVector2D collisionDistance{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGSA_Guardian", meta = (AllowPrivateAccess = true))
	float collisionRadius{};

	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "AIGSA_Guardian", meta = (AllowPrivateAccess = true))
	float speed{};

public:
	AIGSA_Guardian(const FObjectInitializer& _Init);
	void InitSkillActor(const FSkillInitializeParameter& _Param)override;
	void ActiveSkill(float _Duration = 0.0f)override;
	void InactiveSkill()override;
	void InitGuardian(float _Speed);
	void Tick(float _DeltaTime)override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void OnRep_UpdateActive()override;
	void OnConstruction(const FTransform& _Transform)override;

private:

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);
};
