#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_SkillComponent.generated.h"

UCLASS( ClassGroup=(Project_T), meta=(BlueprintSpawnableComponent) )
class PROJECT_T_API UIG_SkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIG_SkillComponent();
	void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;

protected:
	void BeginPlay() override;
	
};
