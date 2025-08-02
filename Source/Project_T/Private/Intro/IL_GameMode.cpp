#include "Intro/IL_GameMode.h"
#include "T_GameInstance.h"
#include "OnlineSessionSettings.h"

namespace INTRO_GAME_MODE
{
	static constexpr const TCHAR* PLAYER_CONTROLLER_PATH{ TEXT("/Game/01_Blueprint/Intro/BP_IL_PlayerController") };
	static const int32 MAX_SEARCH_RESULT{ 20 };
	static const int32 PING_BUCKET_SIZE{ 50 };
}

AIL_GameMode::AIL_GameMode(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> PLAYER_CONTROLLER{ INTRO_GAME_MODE::PLAYER_CONTROLLER_PATH };

	if (PLAYER_CONTROLLER.Succeeded()) PlayerControllerClass = PLAYER_CONTROLLER.Class;
}

void AIL_GameMode::BeginPlay()
{
	Super::BeginPlay();

	// OnlineSubsystem 초기화
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	check(subsystem);
	sessionInterface = subsystem->GetSessionInterface();
	check(sessionInterface.IsValid());

	// 세션 탐색 완료 델리게이트 등록
	findSessionsCompleteHadles.Emplace(sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &AIL_GameMode::OnFindSessionsComplete)));
	sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &AIL_GameMode::OnJoinSessionComplete));
}

void AIL_GameMode::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	// 만약 세션이 남아있다면 정리
	for (auto handle : findSessionsCompleteHadles)
	{
		if (handle.IsValid())
			sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(handle), handle.Reset();
	}
}

void AIL_GameMode::MatchCancel()
{
	// 세션 찾기를 취소한다
	sessionInterface->CancelFindSessions();
	for (auto handle : findSessionsCompleteHadles)
	{
		if (handle.IsValid())
			sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(handle), handle.Reset();
	}
}

bool AIL_GameMode::MatchStart(FOnFindSessionsCompleteDelegate _Find, FOnJoinFailDelegate _JoinFail)
{
	if (!sessionInterface.IsValid())
	{
		return false;
	}
	
	// 세션 탐색 설정 생성
	sessionSearch = MakeShared<FOnlineSessionSearch>();
	if (!sessionSearch)
	{
		return false;
	}

	onJoinFail = _JoinFail;
	sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(_Find);
	sessionSearch->bIsLanQuery = true;
	sessionSearch->MaxSearchResults = INTRO_GAME_MODE::MAX_SEARCH_RESULT;
	sessionSearch->PingBucketSize = INTRO_GAME_MODE::PING_BUCKET_SIZE;

	// 세션 탐색 실행 (LocalPlayerIndex = 0)
	return sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void AIL_GameMode::OnFindSessionsComplete(bool _bWasSuccessful)
{
	if (_bWasSuccessful && sessionSearch.IsValid() && sessionInterface.IsValid())
	{
		// 검색된 세션 중 첫 번째에 자동 접속
		for (const FOnlineSessionSearchResult& searchResult : sessionSearch->SearchResults)
		{
			if (sessionInterface->GetNamedSession(SESSION_NAME) != nullptr)
			{
				FString connectURL{};
				if (sessionInterface->GetResolvedConnectString(SESSION_NAME, connectURL))
				{
					PTT_LOG(Warning, TEXT("Client Travel"));
					GetWorld()->GetFirstPlayerController()->ClientTravel(connectURL, TRAVEL_Absolute);
					return;
				}
				else
				{
					onJoinFail.ExecuteIfBound();
					return;
				}
			}

			if (sessionInterface->JoinSession(0, SESSION_NAME, searchResult))
			{
				PTT_LOG(Warning, TEXT("연결성공"));
				return;
			}
			else
			{
				PTT_LOG(Warning, TEXT("연결실패"));
			}
		}
	}
}

void AIL_GameMode::OnJoinSessionComplete(FName _SessionName, EOnJoinSessionCompleteResult::Type _Result)
{
	if (_Result == EOnJoinSessionCompleteResult::Success && sessionInterface.IsValid())
	{
		FString connectURL{};
		if (sessionInterface->GetResolvedConnectString(_SessionName, connectURL))
		{
			GetWorld()->GetFirstPlayerController()->ClientTravel(connectURL, TRAVEL_Absolute);
			return;
		}
	}

	onJoinFail.ExecuteIfBound();
}
