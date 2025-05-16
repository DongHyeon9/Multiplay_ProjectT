#include "Lobby/L_PlayerController.h"
#include "Lobby/Widget/LW_MatchWait.h"
#include "Lobby/L_GameState.h"

void AL_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() != ENetRole::ROLE_AutonomousProxy) return;
	matchWaitWidget = CreateWidget<ULW_MatchWait>(this, matchWaitClass);
	matchWaitWidget->AddToViewport();
	
	auto gs = GetWorld()->GetGameState<AL_GameState>();
	gs->onLoginPlayer.AddUObject(matchWaitWidget, &ULW_MatchWait::OnLoginPlayer);
	gs->onLogoutPlayer.AddUObject(matchWaitWidget, &ULW_MatchWait::OnLogoutPlayer);
	matchWaitWidget->Init(gs->GetMaxPlayer(), gs->GetCurrentPlayers());
}
