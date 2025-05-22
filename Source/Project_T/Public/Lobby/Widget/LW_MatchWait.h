#pragma once

#include "Lobby/Widget/L_UserWidget.h"
#include "Containers/Ticker.h"
#include "LW_MatchWait.generated.h"

class UButton;
class UHorizontalBox;
class ULW_ConnectState;
class AL_PlayerController;

UCLASS()
class PROJECT_T_API ULW_MatchWait : public UL_UserWidget
{
	GENERATED_BODY()
	
private:
	// 취소 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_MatchWait Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_Cancel{};

	// ULW_ConnectState를 담을 Panel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_MatchWait Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UHorizontalBox> hb_PlayerList{};

	// 최대 플레이어의 수에 따라 동적으로 UI 표시한다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_MatchWait Default", meta = (AllowPrivateAccess = true))
	TArray<ULW_ConnectState*> connectStateList{};

private:
	// 생성될 ULW_ConnectState 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	TSubclassOf<ULW_ConnectState> connectStateClass{};

	// ULW_ConnectState의 사이즈
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	float connectStateSize{ 200.0f };

	// 접속된 플레이어들의 상태를 동기화 하는 주기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	float refreshInterval{ 0.1f };

	// 새로고침 핸들
	FTSTicker::FDelegateHandle refreshHandle{};

public:
	ULW_MatchWait(const FObjectInitializer& _Initializer);
	void NativeConstruct()override;
	// 위젯을 지우기 전 refreshHandle도 지워준다
	void RemoveFromParent()override;
	// 매치 상태를 새로고침한다
	bool RefreshMatchState(float _DeltaTime);

private:
	UFUNCTION()
	void OnClickedCancel();
};
