#pragma once

#include "../Project_T.h"
#include "GameFramework/GameMode.h"
#include "IG_GameMode.generated.h"

UCLASS()
class PROJECT_T_API AIG_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	void OnCompleteAllPlayer();
};
