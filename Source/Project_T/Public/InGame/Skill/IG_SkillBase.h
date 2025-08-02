#pragma once

#include "../Project_T.h"
#include "UObject/Object.h"
#include "IG_SkillBase.generated.h"

class UIG_StatComponent;
class UIG_SkillComponent;
class UNiagaraSystem;

UCLASS(Abstract)
class PROJECT_T_API UIG_SkillBase : public UObject
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(Transient, Replicated, VisibleInstanceOnly, BlueprintReadOnly)
	FSkillInitializeParameter skillInfo{};

	FTimerHandle coolDownHandle{};

	FTimerDelegate timerDelegate{};

private:
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, VisibleAnywhere, category = "UIG_SkillBase", meta = (AllowPrivateAccess = true))
	int32 skillLevel{};

	UPROPERTY()
	UDataTable* skillDataTable{};
	const FSkillData* skillData{};

public:
	UIG_SkillBase(const FObjectInitializer& _Initializer);
	virtual FName GetSkillName()const { return TEXT(""); }
	virtual void InitSkill(const FSkillInitializeParameter& _Initializer);
	virtual void UnRegistSkill();
	virtual void SetSkillLevel(int32 _NewLevel);
	virtual void DestroySkill();
	void LevelUp();

	FORCEINLINE const FSkillData* GetSkillData()const { return skillData; }
	FORCEINLINE int32 GetSkillLevel()const { return skillLevel; }

	UWorld* GetWorld() const override final;
	bool IsSupportedForNetworking() const override { return true; }
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	int32 GetFunctionCallspace(UFunction* _Function, FFrame* _Stack) override;
	bool CallRemoteFunction(UFunction* _Function, void* _Parms, FOutParmRec* _OutParms, FFrame* _Stack) override;

protected:
	virtual void Excute();
	UFUNCTION(NetMulticast, UnReliable)
	virtual void Multicast_Excute();
	virtual void Multicast_Excute_Implementation() {}
	void ApplyDamage(AActor* _Target);
	bool HasAuthority();

private:
	void Excute_Internal();

};
