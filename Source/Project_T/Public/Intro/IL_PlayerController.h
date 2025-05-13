#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "IL_PlayerController.generated.h"

class UILW_Main;

UCLASS()
class PROJECT_T_API AIL_PlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIL_PlayerController", meta = (AllowPrivateAccess = true))
	TSubclassOf<UILW_Main> mainWidgetClass{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIL_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<UILW_Main> mainWidget{};
	
public:
	void BeginPlay()override;
	
};
