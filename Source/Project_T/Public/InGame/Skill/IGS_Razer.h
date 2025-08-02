#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGS_Razer.generated.h"

class USphereComponent;

UCLASS(Blueprintable, BlueprintType)
class PROJECT_T_API UIGS_Razer : public UIG_SkillBase
{
	GENERATED_BODY()
public:
	static const FName DETECTION_COMP_NAME;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Razer", meta = (AllowPrivateAccess = true))
	float distance{ 1300.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Razer", meta = (AllowPrivateAccess = true))
	float detectionRange{ 2000.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Razer", meta = (AllowPrivateAccess = true))
	float range{ 200.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Razer", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraSystem> razerFX{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGS_Razer", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> detectionCollisionComp{};

	UPROPERTY()
	TSet<const AActor*> detectedActors{};

public:
	UIGS_Razer(const FObjectInitializer& _Initializer);
	FName GetSkillName()const override { return TEXT("Razer"); }
	void InitSkill(const FSkillInitializeParameter& _Initializer)override;

protected:
	UFUNCTION(NetMulticast,Unreliable)
	void Multicast_Razer_FX(const FVector_NetQuantizeNormal& _RazerDirection);
	void Multicast_Razer_FX_Implementation(const FVector_NetQuantizeNormal& _RazerDirection);
	void Excute()override;

private:
	FVector ComputeRazerDirection()const;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex);

};
