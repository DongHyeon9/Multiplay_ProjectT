#include "Lobby/L_PlayerState.h"
#include "Lobby/L_PlayerController.h"

void AL_PlayerState::BeginPlay()
{
	Super::BeginPlay();

	auto pc = GetWorld()->GetFirstPlayerController<AL_PlayerController>();
	pc->LoginNewPlayer();
}

void AL_PlayerState::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	auto pc = GetWorld()->GetFirstPlayerController<AL_PlayerController>();
	if (pc)
		pc->LoginNewPlayer();
}
