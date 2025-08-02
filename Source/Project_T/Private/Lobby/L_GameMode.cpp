#include "Lobby/L_GameMode.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

namespace LOBBY_GAME_MODE
{
	static constexpr const TCHAR* PLAYER_CONTROLLER_PATH{ TEXT("/Game/01_Blueprint/Lobby/BP_L_PlayerController") };
	static constexpr const TCHAR* MAIN_LEVEL_PATH{ TEXT("/Game/05_Level/LV_Main.LV_Main") };
}

AL_GameMode::AL_GameMode(const FObjectInitializer& _Initializer) :Super(_Initializer)
{
	bUseSeamlessTravel = true;

	static ConstructorHelpers::FClassFinder<APlayerController> PLAYER_CONTROLLER{ LOBBY_GAME_MODE::PLAYER_CONTROLLER_PATH };

	mainLevel = TSoftObjectPtr<UWorld>(FSoftObjectPath{ LOBBY_GAME_MODE::MAIN_LEVEL_PATH });
	if (PLAYER_CONTROLLER.Succeeded()) PlayerControllerClass = PLAYER_CONTROLLER.Class;
}

void AL_GameMode::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서 세션을 생성한다
	IOnlineSubsystem* subsystem{ IOnlineSubsystem::Get() };
	IOnlineSessionPtr sessions{ subsystem ? subsystem->GetSessionInterface() : nullptr };

	check(sessions.IsValid());

	FOnlineSessionSettings settings{};
	settings.NumPublicConnections = MAX_PLAYER_COUNT;
	settings.bIsLANMatch = true;
	settings.bShouldAdvertise = true;
	settings.bAllowJoinInProgress = true;

	sessions->OnCreateSessionCompleteDelegates.AddUObject(this, &AL_GameMode::OnCreateSessionComplete);
	sessions->CreateSession(0, SESSION_NAME, settings);
}

void AL_GameMode::PreLogin(const FString& _Options, const FString& _Address, const FUniqueNetIdRepl& _UniqueId, FString& _ErrorMessage)
{
	//플레이어가 정원을 초과하면 입장을 거부하고 메세지를 보낸다
	if (GetNumPlayers() >= MAX_PLAYER_COUNT)
	{
		_ErrorMessage = TEXT("Server is full");
		PTT_LOG(Warning, TEXT("%s"), *_ErrorMessage);
		return;
	}
	else
	{
		PTT_LOG(Display, TEXT("Login : %s"), *_UniqueId.ToString());
	}

	Super::PreLogin(_Options, _Address, _UniqueId, _ErrorMessage);
}

void AL_GameMode::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	IOnlineSubsystem* subsystem{ IOnlineSubsystem::Get() };
	IOnlineSessionPtr sessions{ subsystem ? subsystem->GetSessionInterface() : nullptr };

	if (sessions.IsValid())
	{
		sessions->DestroySession(SESSION_NAME);
	}
	else
	{
		PTT_LOG(Error, TEXT("Session InValid"));
	}
}

void AL_GameMode::OnLoginComplete()
{
	//입장 플레이어가 채워지면 ServerTravel을 실행해 게임을 시작한다
	if (MAX_PLAYER_COUNT == GetNumPlayers())
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
	// 세션 생성에 성공했는지 로그를 출력한다
	if (_bWasSuccessful)
	{
		PTT_LOG(Warning, TEXT("Create Session Success : %s"), *_SessionName.ToString());
	}
	else
	{
		PTT_LOG(Warning, TEXT("Create Session Fail"));
	}
}
