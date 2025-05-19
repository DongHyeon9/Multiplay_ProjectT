#include "Lobby/L_PlayerController.h"
#include "Lobby/Widget/LW_MatchWait.h"
#include "Lobby/L_GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "T_GameInstance.h"

void AL_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		matchWaitWidget = CreateWidget<ULW_MatchWait>(this, matchWaitClass);
		matchWaitWidget->AddToViewport();
		SetInputMode(FInputModeUIOnly{});
		SetShowMouseCursor(true);
		Server_SetPlayerName(GetGameInstance<UT_GameInstance>()->GetUserName());
	}
}

void AL_PlayerController::Server_SetPlayerName_Implementation(const FString& _UserName)
{
	GetPlayerState<APlayerState>()->SetPlayerName(_UserName);
	if (auto gm = GetWorld()->GetAuthGameMode<AL_GameMode>())
	{
		gm->OnLoginComplete();
	}
}

bool AL_PlayerController::Server_SetPlayerName_Validate(const FString& _UserName)
{
	return _UserName.StartsWith(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive);
}
