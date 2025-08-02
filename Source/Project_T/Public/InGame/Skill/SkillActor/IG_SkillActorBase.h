#pragma once

#include "../Project_T.h"
#include "GameFramework/Actor.h"
#include "IG_SkillActorBase.generated.h"

UCLASS(Abstract)
class PROJECT_T_API AIG_SkillActorBase : public AActor
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(Transient, Replicated, VisibleInstanceOnly, BlueprintReadOnly, category = "AIG_SkillActorBase")
	FSkillInitializeParameter skillInfo{};

	UPROPERTY(Transient, Replicated, VisibleInstanceOnly, BlueprintReadOnly, category = "AIG_SkillActorBase")
	FSkillActorInfo skillActorInfo{};

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_UpdateActive, VisibleInstanceOnly, BlueprintReadOnly, category = "AIG_SkillActorBase", meta = (AllowPrivateAccess = true))
	bool bIsActive{};

	FTimerHandle activeHandle{};

public:
	AIG_SkillActorBase(const FObjectInitializer& _Init);
	virtual void InitSkillActor(const FSkillInitializeParameter& _Param);
	virtual void ActiveSkill(float _Duration=0.0f);
	virtual void InactiveSkill();
	virtual void SetSkillActorInfo(const FSkillActorInfo& _SkillActorInfo);
	void BeginPlay()override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	FORCEINLINE bool IsActive() { return bIsActive; }

protected:
	UFUNCTION()
	virtual void OnRep_UpdateActive() {}

private:
	void InactiveSkill_Internal();
};
