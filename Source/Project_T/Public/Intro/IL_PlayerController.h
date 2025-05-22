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
	//표시할 메인 위젯의 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIL_PlayerController", meta = (AllowPrivateAccess = true))
	TSubclassOf<UILW_Main> mainWidgetClass{};

	// 생성된 메인위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIL_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<UILW_Main> mainWidget{};

public:
	AIL_PlayerController(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	
};
