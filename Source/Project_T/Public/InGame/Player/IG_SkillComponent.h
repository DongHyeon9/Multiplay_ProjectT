#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "InGame/Skill/IG_SkillBase.h"
#include "IG_SkillComponent.generated.h"

class AIG_SkillBase;

UCLASS(ClassGroup = (Project_T), meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIG_SkillComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "UIG_SkillComponent", meta = (AllowPrivateAccess = true))
	TArray<AIG_SkillBase*> skillList{};

public:
	void RegistSkill(TSubclassOf<AIG_SkillBase> _SkillClass);
	void EndGame();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};