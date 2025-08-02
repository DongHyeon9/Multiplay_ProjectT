#include "Intro/Widget/ILW_Main.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Intro/Widget/ILW_Modal.h"
#include "Components/EditableText.h"
#include "T_GameInstance.h"
#include "Intro/IL_GameMode.h"

#define LOCTEXT_NAMESPACE "UILW_Main"

namespace INTRO_MAIN_WIDGET
{
	static constexpr const TCHAR* MODAL_CLASS_PATH{ TEXT("/Game/01_Blueprint/Intro/Widget/WG_IL_Modal") };
	static const int32 Z_ORDER_MODAL{ 1 };
}

UILW_Main::UILW_Main(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MODAL_WIDGET{ INTRO_MAIN_WIDGET::MODAL_CLASS_PATH };
	if (MODAL_WIDGET.Succeeded())modalWidgetClass = MODAL_WIDGET.Class;
}

void UILW_Main::NativeConstruct()
{
	Super::NativeConstruct();

	btn_StartMatch->OnClicked.AddDynamic(this, &UILW_Main::OnClickedStartMatch);
	btn_Quit->OnClicked.AddDynamic(this, &UILW_Main::OnClickedQuit);
}

void UILW_Main::CreateModal(const FText& _GuideText, const FText& _ButtonText)
{
	if (modalWidget && modalWidget->IsInViewport()) modalWidget->RemoveFromParent();
	// 모달 생성을 모듈화하고
	// 로그를 출력함으로써 반복된 로그 출력 작업을 감소시킴
	PTT_LOG(Warning, TEXT("%s"), *_GuideText.ToString());
	modalWidget = CreateWidget<UILW_Modal>(GetOwningPlayer(), modalWidgetClass);
	modalWidget->SetText(_GuideText, _ButtonText);
	modalWidget->AddToViewport(INTRO_MAIN_WIDGET::Z_ORDER_MODAL);
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
	auto world = GetWorld();
	check(world);

	auto gm = world->GetAuthGameMode<AIL_GameMode>();
	check(gm);
	gm->MatchCancel();
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
	auto gi = GetGameInstance<UT_GameInstance>();
	check(gi);
	gi->SetUserName(playerName);

	auto world = GetWorld();
	check(world);

	auto gm = world->GetAuthGameMode<AIL_GameMode>();
	check(gm);
	if (gm->MatchStart(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UILW_Main::OnFindSessionsComplete), FOnJoinFailDelegate::CreateUObject(this, &UILW_Main::OnJoinFail)))
	{
		CreateModal(LOCTEXT("Finding Session", "서버 접속 중..."), LOCTEXT("Cancel Find Session", "취소"));
		modalWidget->onClicked.AddDynamic(this, &UILW_Main::OnClickedMatchCancel);
	}
	else
	{
		CreateModal(LOCTEXT("Session Error Text", "서버에 접속할 수 없습니다.\n인터넷 연결을 확인해 주세요."));
	}
}

void UILW_Main::OnClickedQuit()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UILW_Main::OnFindSessionsComplete(bool _bWasSuccessful)
{
	RemoveModal();

	if (!_bWasSuccessful)
	{
		CreateModal(LOCTEXT("Session Error Text", "서버에 접속할 수 없습니다.\n인터넷 연결을 확인해 주세요."));
	}
}

void UILW_Main::OnJoinFail()
{
	RemoveModal();
	CreateModal(LOCTEXT("TravelError", "접속 실패."));
}

#undef LOCTEXT_NAMESPACE