#pragma once

#include "../Project_T.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "IG_EnemyMovement.generated.h"

UCLASS()
class PROJECT_T_API UIG_EnemyMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()
public:
	UIG_EnemyMovement(const FObjectInitializer& _Initializer);
	void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;
};
