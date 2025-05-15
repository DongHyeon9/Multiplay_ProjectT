#include "InGame/Player/IG_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AIG_PlayerController::AIG_PlayerController(const FObjectInitializer& _Initializer):
	Super(_Initializer)
{
	playerMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("PlayerMappingContext"));
}

void AIG_PlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//playerMappingContext 설정하는 로직
}

void AIG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly{});
	SetShowMouseCursor(false);

	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		subsystem->AddMappingContext(playerMappingContext, 0);
	}
}

void AIG_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
