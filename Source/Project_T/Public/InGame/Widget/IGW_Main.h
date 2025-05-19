#pragma once

#include "InGame/Widget/IG_UserWidget.h"
#include "IGW_Main.generated.h"

class UWidgetAnimation;
class AIG_GameState;
class UTextBlock;

UCLASS()
class PROJECT_T_API UIGW_Main : public UIG_UserWidget
{
	GENERATED_BODY()
public:
	FWidgetAnimationDynamicEvent onFinishStartAnim{};
	FWidgetAnimationDynamicEvent onFinishEndAnim{};

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_CurrentTime{};

private:
	UPROPERTY(Transient, BlueprintReadOnly, category = "UIGW_Main", meta = (BindWidgetAnim, AllowPrivateAccess = true))
	UWidgetAnimation* startWidgetAnim{};

	UPROPERTY(Transient, BlueprintReadOnly, category = "UIGW_Main", meta = (BindWidgetAnim, AllowPrivateAccess = true))
	UWidgetAnimation* endWidgetAnim{};

private:
	UPROPERTY()
	TObjectPtr<AIG_GameState> gameState{};

public:
	void InitWidget();
	void InitTimer(TObjectPtr<AIG_GameState> _GameState);
	void StartGame();
	void EndGame();
	void NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)override;
};
