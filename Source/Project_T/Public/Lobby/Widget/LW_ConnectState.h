#pragma once

#include "Lobby/Widget/L_UserWidget.h"
#include "LW_ConnectState.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECT_T_API ULW_ConnectState : public UL_UserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_ConnectState", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UImage> img_PlayerImage{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_ConnectState", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_PlayerName{};

public:
	// 위젯의 표시 상태를 변경한다
	// @param _PlayerState nullptr이면 접속 대기중...으로 표시된다
	void SetState(const APlayerState* _PlayerState = nullptr);
};
