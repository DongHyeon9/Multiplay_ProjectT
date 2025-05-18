#include "Lobby/L_PlayerController.h"
#include "Lobby/Widget/LW_MatchWait.h"
#include "Lobby/L_GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void AL_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		matchWaitWidget = CreateWidget<ULW_MatchWait>(this, matchWaitClass);
		matchWaitWidget->AddToViewport();
		matchWaitWidget->Init(maxPlayer);
	}
	else if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		auto gm = GetWorld()->GetAuthGameMode<AL_GameMode>();
		gm->onUpdateMatchState.AddUObject(this, &AL_PlayerController::Client_UpdateMatchState);
		maxPlayer = gm->GetMaxPlayer();
	}
}

void AL_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AL_PlayerController, maxPlayer, COND_InitialOnly);
}

void AL_PlayerController::LoginNewPlayer()
{
	if (!matchWaitWidget) return;
	matchWaitWidget->OnStateChange();
}

void AL_PlayerController::Client_UpdateMatchState_Implementation()
{
	LoginNewPlayer();
}
