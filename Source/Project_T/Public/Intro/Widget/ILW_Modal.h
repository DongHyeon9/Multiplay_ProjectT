#pragma once

#include "Intro/Widget/IL_UserWidget.h"
#include "Components/Button.h"
#include "ILW_Modal.generated.h"

class UTextBlock;

UCLASS()
class PROJECT_T_API UILW_Modal : public UIL_UserWidget
{
	GENERATED_BODY()
public:
	FOnButtonClickedEvent onClicked{};

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_Guide{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_ButtonText{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_OK{};

public:
	void NativeConstruct()override;
	void SetText(const FText& _GuideText, const FText& _ButtonText);

private:
	UFUNCTION()
	void OnClicked();
	
};
