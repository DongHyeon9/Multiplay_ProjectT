#pragma once

#include "IG_Enum.h"
#include "Engine/DataTable.h"
#include "IG_Struct.generated.h"

USTRUCT(BlueprintType, Atomic)
struct FPlayerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString playerName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor playerColor{};

	FString ToString()const
	{
		return FString::Printf(TEXT("%s : %s"), *playerName, *playerColor.ToString());
	}
};

USTRUCT(BlueprintType, Atomic)
struct FCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float maxHP{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float defaultDamage{};
};

USTRUCT(BlueprintType, Atomic)
struct FCharacterData_Player : public FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float needEXP{};
};

USTRUCT(BlueprintType, Atomic)
struct FCharacterData_Enemy : public FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float dropEXP{};
};

USTRUCT(Atomic, BlueprintType)
struct FDamageCompParameter
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UShapeComponent> ownerCollision{};

	// 플레이와 Overlap시 데미지를 주는 간격
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "FDamageCompParameter")
	float damageInterval{ 0.1f };
};

USTRUCT(BlueprintType, Atomic)
struct FSkillData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<float> skillDamages{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<float> skillCoolDown{};
};

USTRUCT(Atomic)
struct FSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName skillName{};
};

USTRUCT(BlueprintType, Atomic)
struct FSkillInfo_Client : public FSkillInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText displayName{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText description{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* skillIcon{};
};

USTRUCT(BlueprintType, Atomic)
struct FSkillInfo_Server : public FSkillInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UIG_SkillBase> skillClass{};
};

struct FSkillInfoParam
{
	FSkillInfoParam(const FSkillInfo_Client* _Info, int32 _Level)
	{
		skillName = _Info->skillName;
		displayName = _Info->displayName;
		description = _Info->description;
		skillIcon = _Info->skillIcon;
		skillLevel = _Level;
	}

	FName skillName{};
	FText displayName{};
	FText description{};
	UTexture2D* skillIcon{};
	int32 skillLevel{};
};

USTRUCT(BlueprintType, Atomic)
struct FSkillInitializeParameter
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<class APawn> owner{};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<class UIG_SkillComponent> ownerComp{};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<class UIG_StatComponent> ownerStat{};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> collisionType{};
};

USTRUCT(BlueprintType, Atomic)
struct FSkillActorInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 skillLevel{};

	UPROPERTY(NotReplicated, VisibleInstanceOnly, BlueprintReadOnly)
	float damage{};
};