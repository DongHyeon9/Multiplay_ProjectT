#include "Intro/Widget/ILW_Main.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Intro/Widget/ILW_Modal.h"
#include "Components/EditableText.h"
#include "GameFramework/PlayerState.h"
#include "T_GameInstance.h"

#define LOCTEXT_NAMESPACE "UILW_Main"

void UILW_Main::NativeConstruct()
{
	Super::NativeConstruct();

	btn_StartMatch->OnClicked.AddDynamic(this, &UILW_Main::OnClickedStartMatch);
	btn_Quit->OnClicked.AddDynamic(this, &UILW_Main::OnClickedQuit);

	// OnlineSubsystem 초기화
	if (IOnlineSubsystem* subsystem = IOnlineSubsystem::Get())
	{
		sessionInterface = subsystem->GetSessionInterface();
		if (!sessionInterface.IsValid()) return;
		// 세션 탐색 완료 델리게이트 등록
		findSessionsComplete = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UILW_Main::OnFindSessionsComplete));
		sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UILW_Main::OnJoinSessionComplete));
	}
}

void UILW_Main::CreateModal(const FText& _GuideText, const FText& _ButtonText)
{
	if (modalWidget && modalWidget->IsInViewport()) modalWidget->RemoveFromParent();

	modalWidget = CreateWidget<UILW_Modal>(GetOwningPlayer(), modalWidgetClass);
	modalWidget->SetText(_GuideText, _ButtonText);
	modalWidget->AddToViewport(1);
}

void UILW_Main::RemoveModal()
{
	if (!modalWidget || !modalWidget->IsInViewport()) return;
	modalWidget->onClicked.RemoveDynamic(this, &UILW_Main::OnClickedMatchCancel);
	modalWidget->RemoveFromParent();
}

void UILW_Main::OnClickedMatchCancel()
{
	RemoveModal();
	sessionInterface->CancelFindSessions();
	sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsComplete);
	CreateModal(LOCTEXT("Find Cancel CallBack", "서버 접속을 취소했습니다."));
}

void UILW_Main::OnClickedStartMatch()
{
	FString playerName{ edt_Name->GetText().ToString() };
	if (playerName.IsEmpty())
	{
		CreateModal(LOCTEXT("Write Name", "플레이어 이름을 작성해 주세요."));
		return;
	}

	if (!sessionInterface.IsValid())
	{
		CreateModal(LOCTEXT("Session Error Text", "서버에 접속할 수 없습니다.\n인터넷 연결을 확인해 주세요."));
		return;
	}
	GetGameInstance<UT_GameInstance>()->SetUserName(playerName);
	CreateModal(LOCTEXT("Finding Session", "서버 접속 중..."), LOCTEXT("Cancel Find Session", "취소"));
	modalWidget->onClicked.AddDynamic(this, &UILW_Main::OnClickedMatchCancel);

	// 세션 탐색 설정 생성
	sessionSearch = MakeShared<FOnlineSessionSearch>();
	sessionSearch->bIsLanQuery = true;
	sessionSearch->MaxSearchResults = 20;
	sessionSearch->PingBucketSize = 50;

	// 세션 탐색 실행 (LocalPlayerIndex = 0)
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UILW_Main::OnClickedQuit()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UILW_Main::OnFindSessionsComplete(bool _bWasSuccessful)
{
	RemoveModal();

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
					GetOwningPlayer()->ClientTravel(connectURL, TRAVEL_Absolute);
					return;
				}
				else
				{
					CreateModal(LOCTEXT("TravelError", "접속 실패."));
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

	CreateModal(LOCTEXT("Session Error Text", "서버에 접속할 수 없습니다.\n인터넷 연결을 확인해 주세요."));
}

void UILW_Main::OnJoinSessionComplete(FName _SessionName, EOnJoinSessionCompleteResult::Type _Result)
{
	if (_Result == EOnJoinSessionCompleteResult::Success && sessionInterface.IsValid())
	{
		FString connectURL{};
		if (sessionInterface->GetResolvedConnectString(_SessionName, connectURL))
		{
			GetOwningPlayer()->ClientTravel(connectURL, TRAVEL_Absolute);
			return;
		}
	}

	CreateModal(LOCTEXT("TravelError", "접속 실패."));
}

#undef LOCTEXT_NAMESPACE