#include "Lobby/L_GameMode.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

AL_GameMode::AL_GameMode(const FObjectInitializer& _Initializer):Super(_Initializer)
{
	bUseSeamlessTravel = true;
}

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

void AL_GameMode::PreLogin(const FString& _Options, const FString& _Address, const FUniqueNetIdRepl& _UniqueId, FString& _ErrorMessage)
{
	if (GetNumPlayers() >= maxPlayer)
	{
		_ErrorMessage = TEXT("Server is full");
		return;
	}

	Super::PreLogin(_Options, _Address, _UniqueId, _ErrorMessage);
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

void AL_GameMode::OnLoginComplete()
{
	if (maxPlayer == GetNumPlayers())
	{
		const FString mapURL = mainLevel.GetLongPackageName() + TEXT("?listen");

		if (GetWorld()->ServerTravel(mapURL, true))
		{
			PTT_LOG(Warning, TEXT("Success Travel Level : %s"), *mapURL);
		}
		else
		{
			PTT_LOG(Warning, TEXT("Fail Travel Level : %s"), *mapURL);
		}
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
