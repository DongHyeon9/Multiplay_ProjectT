#pragma once

#include "InGame/Component/IG_StatComponent.h"
#include "IG_PlayerStatComponent.generated.h"

UCLASS(ClassGroup = (Project_T), meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIG_PlayerStatComponent : public UIG_StatComponent
{
	GENERATED_BODY()
public:
	FOnEXPChangeDelegate onEXPChange{};
	FOnLevelChangeDelegate onLevelChange{};

protected:

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LevelChange, BlueprintReadOnly, VisibleAnywhere, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	int32 playerLevel{};

	UPROPERTY(Transient, ReplicatedUsing = OnRep_EXPChange, BlueprintReadOnly, VisibleAnywhere, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	float currentEXP{};

public:
	UIG_PlayerStatComponent(const FObjectInitializer& _Initializer);
	FORCEINLINE int32 GetPlayerLevel()const { return playerLevel; }
	FORCEINLINE float GetCurrentEXP()const { return currentEXP; }
#pragma region 서버 전용 코드
	void InitStat(int32 _Idx)override;
	void ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage) override;
	void SetEXP(float _NewEXP);
	void AddEXP(float _AddEXP);
	void SetLevel(int32 _NewLevel);
	void LevelUp(int32 _AddLevel = 1);
#pragma endregion 서버 전용 코드

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

private:
	void SetDataTable(int32 _Idx)override { Super::SetDataTable(_Idx); }
	UFUNCTION()	void OnRep_EXPChange(float _Prev);
	UFUNCTION()	void OnRep_LevelChange(int32 _Prev);
};