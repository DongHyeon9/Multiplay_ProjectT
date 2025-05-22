#include "Intro/IL_GameMode.h"

AIL_GameMode::AIL_GameMode(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> PLAYER_CONTROLLER(TEXT("/Game/01_Blueprint/Intro/BP_IL_PlayerController"));

	if (PLAYER_CONTROLLER.Succeeded()) PlayerControllerClass = PLAYER_CONTROLLER.Class;
}
