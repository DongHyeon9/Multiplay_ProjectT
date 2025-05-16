#include "Intro/Widget/ILW_Main.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Intro/Widget/ILW_Modal.h"
#include "Components/EditableText.h"
#include "GameFramework/PlayerState.h"

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
	modalWidget->onClicked.RemoveDynamic(this, &UILW_Main::OnClickedMatchCancel);
	modalWidget->RemoveFromParent();
}

void UILW_Main::OnClickedMatchCancel()
{
	RemoveModal();
	sessionInterface->CancelFindSessions();
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

	GetOwningPlayer()->GetPlayerState<APlayerState>()->SetPlayerName(playerName);
	CreateModal(LOCTEXT("Finding Session", "서버 접속 중..."), LOCTEXT("Cancel Find Session", "취소"));
	modalWidget->onClicked.AddDynamic(this, &UILW_Main::OnClickedMatchCancel);

	// 세션 탐색 설정 생성
	sessionSearch = MakeShared<FOnlineSessionSearch>();
	sessionSearch->bIsLanQuery = false;
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
			// 조인 시도 (SessionName은 기본값)
			sessionInterface->JoinSession(0, NAME_GameSession, searchResult);
			return;
		}
	}

	CreateModal(LOCTEXT("Session Error Text", "서버에 접속할 수 없습니다.\n인터넷 연결을 확인해 주세요."));
}

#undef LOCTEXT_NAMESPACE