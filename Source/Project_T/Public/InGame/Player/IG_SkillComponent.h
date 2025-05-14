#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "InGame/Skill/IG_SkillBase.h"
#include "IG_SkillComponent.generated.h"

class AIG_SkillBase;

UCLASS( ClassGroup=(Project_T), meta=(BlueprintSpawnableComponent) )
class PROJECT_T_API UIG_SkillComponent : public UActorComponent
{
	GENERATED_BODY()
public:

protected:
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<AIG_SkillBase>> SkillClasses;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AIG_SkillBase>> SkillInstances;
public:	
	UIG_SkillComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void UseSkill(E_SKILL_TYPE SkillType);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;
};
