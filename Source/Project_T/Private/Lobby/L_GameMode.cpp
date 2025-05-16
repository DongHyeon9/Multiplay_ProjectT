#include "Lobby/L_GameMode.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

static FName SESSION_NAME{ TEXT("ProjectTSession") };

void AL_GameMode::BeginPlay()
{
	Super::BeginPlay();

	IOnlineSubsystem* subsystem{ IOnlineSubsystem::Get() };
	IOnlineSessionPtr sessions{ subsystem ? subsystem->GetSessionInterface() : nullptr };

	check(sessions.IsValid());

	FOnlineSessionSettings settings{};
	settings.NumPublicConnections = 5;
	settings.bIsLANMatch = true;
	settings.bShouldAdvertise = true;
	settings.bAllowJoinInProgress = true;
	connectedPlayers.Reserve(maxPlayer);

	sessions->OnCreateSessionCompleteDelegates.AddUObject(this, &AL_GameMode::OnCreateSessionComplete);
	sessions->CreateSession(0, SESSION_NAME, settings);
}

void AL_GameMode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);

	connectedPlayers.Emplace(_NewPlayer);
	onLoginPlayer.Broadcast(_NewPlayer);
	PTT_LOG(Warning, TEXT("Current Player Count : %d"), GetNumPlayers());

	//인원수 채워지면 시작하는 로직
}

void AL_GameMode::Logout(AController* _Exiting)
{
	Super::Logout(_Exiting);

	if (APlayerController* pc = Cast<APlayerController>(_Exiting))
	{
		connectedPlayers.Remove(pc);
		onLogoutPlayer.Broadcast(pc);
	}
}

void AL_GameMode::OnCreateSessionComplete(FName _SessionName, bool _bWasSuccessful)
{
	if (_bWasSuccessful)
	{
		PTT_LOG(Warning, TEXT("Create Session Success : %s"), *_SessionName.ToString());
	}
	else
	{
		PTT_LOG(Warning, TEXT("Create Session Fail"));
	}
}
