#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "L_PlayerController.generated.h"

class ULW_MatchWait;

UCLASS()
class PROJECT_T_API AL_PlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_PlayerController", meta = (AllowPrivateAccess = true))
	TSubclassOf<ULW_MatchWait> matchWaitClass{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AL_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<ULW_MatchWait> matchWaitWidget{};

public:
	void BeginPlay()override;
};
