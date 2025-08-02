#pragma once

#include "InGame/Skill/SkillActor/IG_SkillActorBase.h"
#include "IGSA_Projectile.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class PROJECT_T_API AIGSA_Projectile : public AIG_SkillActorBase
{
	GENERATED_BODY()
public:
	const static FName ROOT_COMP_NAME;
	const static FName MESH_COMP_NAME;
	const static FName COLLISION_COMP_NAME;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category="AIGSA_Projectile", meta=(AllowPrivateAccess=true))
	TObjectPtr<USceneComponent> rootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category="AIGSA_Projectile", meta=(AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> meshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category="AIGSA_Projectile", meta=(AllowPrivateAccess=true))
	TObjectPtr<USphereComponent> collisionComp;
	
	float speed{};
	FVector direction{};

public:
	AIGSA_Projectile(const FObjectInitializer& _Init);
	void InitSkillActor(const FSkillInitializeParameter& _Param)override;
	void BeginPlay()override;
	void InitProjectile(float _Speed);
	void ActiveSkill(float _Duration=0.0f)override;
	void InactiveSkill()override;
	void Tick(float _DeltaTime)override;

protected:
	void OnRep_UpdateActive()override;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);
	
};
