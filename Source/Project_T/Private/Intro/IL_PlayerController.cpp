#include "Intro/IL_PlayerController.h"
#include "Intro/Widget/ILW_Main.h"

AIL_PlayerController::AIL_PlayerController(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MAIN_WIDGET(TEXT("/Game/01_Blueprint/Intro/Widget/WG_IL_Main"));

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
