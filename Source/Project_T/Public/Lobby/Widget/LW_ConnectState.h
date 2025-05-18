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
	void SetState(const APlayerState* _PlayerState = nullptr);
};
