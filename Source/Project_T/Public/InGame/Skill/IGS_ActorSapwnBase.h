#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGS_ActorSapwnBase.generated.h"

class AIG_SkillActorBase;

UCLASS(Abstract)
class PROJECT_T_API UIGS_ActorSapwnBase : public UIG_SkillBase
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_ActorSapwnBase")
	TSubclassOf<AIG_SkillActorBase> skillActorClass{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_ActorSapwnBase")
	TArray<float> durations{};

private:
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, category = "UIGS_ActorSapwnBase", meta = (AllowPrivateAccess = true))
	TArray<AIG_SkillActorBase*> skillActorPool{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_ActorSapwnBase", meta = (AllowPrivateAccess = true))
	int32 poolSize{};

public:
	void SetSkillLevel(int32 _NewLevel)override;

protected:
	void InitSkill(const FSkillInitializeParameter& _Initializer)override;
	void DestroySkill()override;

	template <class _T>
	_T* GetSkillActor()
	{
		for (auto skillActor : skillActorPool)
		{
			if (!skillActor->IsActive())
				return Cast<_T>(skillActor);
		}

		ExpandPool();
		return GetSkillActor<_T>();
	}

private:
	void ExpandPool();
};
