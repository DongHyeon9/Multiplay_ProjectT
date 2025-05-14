// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IG_SkillBase.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	E_SKILL_TYPE Type{ E_SKILL_TYPE::LINETRACE };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FName Name{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	uint8 level{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float damage{ 10.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float coolTime{ 5.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float duration{ 3.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TObjectPtr<UAnimMontage> skillMontage{};
};

UCLASS(Abstract, Blueprintable)
class PROJECT_T_API AIG_SkillBase : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = true), Replicated)
	FSkillInfo SkillInfo;
protected:

private:


	UPROPERTY()
	AActor* OwnerActor;
public:	
	AIG_SkillBase();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual void UseSkill(); 

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
};
