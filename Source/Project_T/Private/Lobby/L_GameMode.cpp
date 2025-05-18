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

	sessions->OnCreateSessionCompleteDelegates.AddUObject(this, &AL_GameMode::OnCreateSessionComplete);
	sessions->CreateSession(0, SESSION_NAME, settings);
}

void AL_GameMode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);

	onUpdateMatchState.Broadcast();

	//플레이어가 다 차면 서버트래블
}

void AL_GameMode::Logout(AController* _Controller)
{
	Super::Logout(_Controller);

	onUpdateMatchState.Broadcast();
}

void AL_GameMode::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	if (_Reason != EEndPlayReason::LevelTransition)
	{
		IOnlineSubsystem* subsystem{ IOnlineSubsystem::Get() };
		IOnlineSessionPtr sessions{ subsystem ? subsystem->GetSessionInterface() : nullptr };

		check(sessions.IsValid());
		sessions->DestroySession(SESSION_NAME);
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
