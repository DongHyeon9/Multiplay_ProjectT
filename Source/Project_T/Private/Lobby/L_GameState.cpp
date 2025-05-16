#include "Lobby/L_GameState.h"
#include "Lobby/L_GameMode.h"
#include "Net/UnrealNetwork.h"

void AL_GameState::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		auto gm = GetWorld()->GetAuthGameMode<AL_GameMode>();
		gm->onLoginPlayer.AddUObject(this, &AL_GameState::Client_LoginPlayer);
		gm->onLogoutPlayer.AddUObject(this, &AL_GameState::Client_LogoutPlayer);
		maxPlayer = gm->GetMaxPlayer();
		Client_Init_Implementation(maxPlayer, connectedPlayers);
	}
}

void AL_GameState::Client_Init_Implementation(int32 _MaxPlayer, const TArray<APlayerController*>& _ConnectedPlayers)
{
	connectedPlayers = _ConnectedPlayers;
	maxPlayer = _MaxPlayer;
}

void AL_GameState::Client_LoginPlayer_Implementation(const APlayerController* _LoginPlayer)
{
	onLoginPlayer.Broadcast(_LoginPlayer);
}

void AL_GameState::Client_LogoutPlayer_Implementation(const APlayerController* _LoginPlayer)
{
	onLogoutPlayer.Broadcast(_LoginPlayer);
}
