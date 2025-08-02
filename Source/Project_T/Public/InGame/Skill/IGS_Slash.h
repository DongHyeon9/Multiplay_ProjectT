#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGS_Slash.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECT_T_API UIGS_Slash : public UIG_SkillBase
{
	GENERATED_BODY()
public:
	FName GetSkillName()const override { return TEXT("Slash"); }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Slash", meta = (AllowPrivateAccess = true))
	float baseRange{ 300.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Slash", meta = (AllowPrivateAccess = true))
	float rangeOfLevel{ 100.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Slash", meta = (AllowPrivateAccess = true))
	FVector baseSacle{ 0.5f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Slash", meta = (AllowPrivateAccess = true))
	FVector scaleOfLevel{ 0.16666f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Slash", meta = (AllowPrivateAccess = true))
	FRotator rotationOffset{ 0.0f,-90.0f,0.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Slash", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraSystem> slashFX{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Slash", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraSystem> hitFX{};

public:
	UIGS_Slash(const FObjectInitializer& _Initializer);

protected:
	void Multicast_Excute_Implementation()override;
	void Excute()override;

private:
	bool DetectTarget(TArray<FHitResult>& _OutResult);

};