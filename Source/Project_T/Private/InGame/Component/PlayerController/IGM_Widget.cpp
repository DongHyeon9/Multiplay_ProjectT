#include "InGame/Component/PlayerController/IGM_Widget.h"
#include "InGame/Component/Player/IG_SkillComponent.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/Widget/IGW_ResultPopup.h"
#include "InGame/Widget/IGW_Main.h"
#include "InGame/IG_GameState.h"
#include "GameFramework/PlayerState.h"

namespace INGAME_WIDGET_MANAGER
{
	static constexpr const TCHAR* MAIN_WIDGET_PATH{ TEXT("/Game/01_Blueprint/InGame/Widget/WG_IGW_Main") };
	static constexpr const TCHAR* RESULT_POPUP_PATH{ TEXT("/Game/01_Blueprint/InGame/Widget/WG_IGW_ResultPopup") };

	namespace Z_ORDER
	{
		static constexpr int32 MAIN{ 0 };
		static constexpr int32 RESULT{ 1 };
	}
}

UIGM_Widget::UIGM_Widget(const FObjectInitializer& _Init)
	:Super(_Init)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MAIN_WIDGET(INGAME_WIDGET_MANAGER::MAIN_WIDGET_PATH);
	static ConstructorHelpers::FClassFinder<UUserWidget> RESULT_POPUP(INGAME_WIDGET_MANAGER::RESULT_POPUP_PATH);

	if (MAIN_WIDGET.Succeeded()) mainWidgetClass = MAIN_WIDGET.Class;
	if (RESULT_POPUP.Succeeded()) resultPopupClass = RESULT_POPUP.Class;

}

FWidgetAnimationDynamicEvent& UIGM_Widget::MainWidget_GetOnFinishStartAnim()
{
	return mainWidget->onFinishStartAnim;
}

void UIGM_Widget::MainWidget_StartGameAnim()
{
	mainWidget->StartGame();
}

void UIGM_Widget::MainWidget_EndGameAnim()
{
	mainWidget->EndGame();
}

void UIGM_Widget::MainWidget_Init()
{
	mainWidget->InitWidget(GetWorld()->GetGameState<AIG_GameState>());
}

void UIGM_Widget::MainWidget_SetName(const FString& _NewName)
{
	mainWidget->SetName(_NewName);
}

void UIGM_Widget::MainWidget_BindStat(const TObjectPtr<UIG_PlayerStatComponent> _StatComp)
{
	mainWidget->BindStat(_StatComp);
}

void UIGM_Widget::MainWidget_Client_SkillAcquisition_Implementation(const TArray<FName>& _SkillList)
{
	auto pc{ GetPlayerController() };

	// 스킬을 배울 수 있는 UI 띄우기
	pc->SetInputMode(FInputModeGameAndUI{});
	pc->SetShowMouseCursor(true);

	auto skillComp{ GetPlayerCharacter()->GetSkillComp() };
	check(skillComp);

	TArray<FSkillInfoParam> skillInfo{};
	for (auto skillName : _SkillList)
	{
		FSkillInfoParam param{ skillComp->GetSkillInfo_Client(skillName) ,skillComp->GetSkillLevel(skillName) };
		skillInfo.Emplace(param);
	}

	mainWidget->InitSkillSelect(skillInfo);
}

void UIGM_Widget::BeginPlay()
{
	Super::BeginPlay();

	auto pc{ GetPlayerController() };

	// 위젯을 생성한다하고 초기화한다
	mainWidget = CreateWidget<UIGW_Main>(pc, mainWidgetClass);
	mainWidget->onFinishEndAnim.BindDynamic(this, &UIGM_Widget::ShowGameResult);
	mainWidget->AddToViewport(INGAME_WIDGET_MANAGER::Z_ORDER::MAIN);

	resultWidget = CreateWidget<UIGW_ResultPopup>(pc, resultPopupClass);

	// 인풋 모드를 변경한다
	pc->SetInputMode(FInputModeGameOnly{});
	pc->SetShowMouseCursor(false);
}

void UIGM_Widget::ShowGameResult()
{
	auto gs = GetWorld()->GetGameState<AIG_GameState>();

	Algo::Sort(gs->PlayerArray, [](const TObjectPtr<APlayerState>& _Lhs, const TObjectPtr<APlayerState>& _Rhs) {return _Lhs->GetScore() < _Rhs->GetScore();	});

	resultWidget->InitResultPopup(gs->PlayerArray);
	resultWidget->AddToViewport(INGAME_WIDGET_MANAGER::Z_ORDER::RESULT);
}
