#include "Intro/IL_PlayerController.h"
#include "Intro/Widget/ILW_Main.h"

void AIL_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	mainWidget = CreateWidget<UILW_Main>(this, mainWidgetClass);
	mainWidget->AddToViewport();
	FInputModeUIOnly inputMode{};
	SetInputMode(inputMode);
	SetShowMouseCursor(true);
}
