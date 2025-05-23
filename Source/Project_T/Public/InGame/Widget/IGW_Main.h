#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "IGW_Main.generated.h"

class UWidgetAnimation;
class AIG_GameState;
class UTextBlock;

UCLASS()
class PROJECT_T_API UIGW_Main : public UUserWidget
{
	GENERATED_BODY()
public:
	// 게임시작 애니메이션이 끌날때 호출되는 델리게이트
	FWidgetAnimationDynamicEvent onFinishStartAnim{};
	// 게임종료 애니메이션이 끝날때 호출되는 델리게이트
	FWidgetAnimationDynamicEvent onFinishEndAnim{};

private:
	// 현재 게임이 진행된 시간을 표시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_CurrentTime{};

private:
	// 게임시작 애니메이션
	UPROPERTY(Transient, BlueprintReadOnly, category = "UIGW_Main", meta = (BindWidgetAnim, AllowPrivateAccess = true))
	UWidgetAnimation* startWidgetAnim{};

	// 게임종료 애니메이션
	UPROPERTY(Transient, BlueprintReadOnly, category = "UIGW_Main", meta = (BindWidgetAnim, AllowPrivateAccess = true))
	UWidgetAnimation* endWidgetAnim{};

private:
	// 현재 게임의 게임스테이트(진행시간 동기화용)
	UPROPERTY()
	TObjectPtr<AIG_GameState> gameState{};

public:
	// 애니메이션 종료 델리게이트를 애니메이션에 등록한다
	void InitWidget();
	// 타이머를 초기화 한다
	void InitTimer(TObjectPtr<AIG_GameState> _GameState);
	// 게임시작 애니메이션을 재생한다
	void StartGame();
	// 게임종료 애니메이션을 재생하고 타이머를 숨긴다
	void EndGame();
	void NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)override;
};
