#pragma once

#include "../Project_T.h"
#include "UObject/NoExportTypes.h"
#include "IG_SkillBase.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class E_SKILL_TYPE : uint8
{
	LINETRACE = 0	UMETA(DisplayName = "LineTrace"),
	SPIN			UMETA(DisplayName = "Spin"),
	AOE				UMETA(DisplayName = "AreaofEffect"),
	CHAIN			UMETA(DisplayName = "Chain"),

	END,
};

USTRUCT(Atomic, BlueprintType)
struct FSkillInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FName skillName{};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TObjectPtr<AActor> skillActor{};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> skillMontage{};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	float coolTime{ 5.f };

};

UCLASS()
class PROJECT_T_API UIG_SkillBase : public UObject
{
	GENERATED_BODY()
public:

protected:

private:
	UPROPERTY(EditAnywhere,Replicated,Category = "SkillInfo")
	TArray<FSkillInfo> skillInfos;

public:
	UFUNCTION()
	void UseSkill();
	UFUNCTION(BlueprintCallable,Category="Skill")
	void UseSkill_Implementation();
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const  override;
private:

};
