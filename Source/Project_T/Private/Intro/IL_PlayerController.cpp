#include "Intro/IL_PlayerController.h"
#include "Intro/Widget/ILW_Main.h"

namespace INTRO_PLAYER_CONTROLLER
{
	static constexpr const TCHAR* MAIN_WIDGET_PATH{ TEXT("/Game/01_Blueprint/Intro/Widget/WG_IL_Main") };
}

AIL_PlayerController::AIL_PlayerController(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MAIN_WIDGET{ INTRO_PLAYER_CONTROLLER::MAIN_WIDGET_PATH };

	if (MAIN_WIDGET.Succeeded()) mainWidgetClass = MAIN_WIDGET.Class;
}

void AIL_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	mainWidget = CreateWidget<UILW_Main>(this, mainWidgetClass);
	mainWidget->AddToViewport();
	FInputModeUIOnly inputMode{};
	SetInputMode(inputMode);
	SetShowMouseCursor(true);
}
