#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "IG_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECT_T_API AIG_PlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_PlayerController Input", meta = (AllowPrivateAccess = true))
	UInputMappingContext* playerMappingContext{};

public:
	AIG_PlayerController(const FObjectInitializer& _Initializer);
	void PreInitializeComponents()override;

protected:
	void BeginPlay()override;
	void SetupInputComponent()override;

};
