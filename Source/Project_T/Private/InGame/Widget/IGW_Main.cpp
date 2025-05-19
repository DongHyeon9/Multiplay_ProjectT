#include "InGame/Widget/IGW_Main.h"
#include "InGame/IG_GameState.h"
#include "Components/TextBlock.h"

void UIGW_Main::InitWidget()
{
	BindToAnimationFinished(startWidgetAnim, onFinishStartAnim);
	BindToAnimationFinished(endWidgetAnim, onFinishEndAnim);
}

void UIGW_Main::InitTimer(TObjectPtr<AIG_GameState> _GameState)
{
	gameState = _GameState;
	txt_CurrentTime->SetVisibility(ESlateVisibility::Visible);
}

void UIGW_Main::StartGame()
{
	PlayAnimation(startWidgetAnim);
}

void UIGW_Main::EndGame()
{
	txt_CurrentTime->SetVisibility(ESlateVisibility::Hidden);
	PlayAnimation(endWidgetAnim);
}

void UIGW_Main::NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)
{
	Super::NativeTick(_MyGeometry, _InDeltaTime);

	if (gameState)
	{
		txt_CurrentTime->SetText(FText::FromString(FString::Printf(TEXT("남은 시간 : %d"), gameState->GetTimer())));
	}
}
