#pragma once

#include "Intro/Widget/IL_UserWidget.h"
#include "ILW_Modal.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class PROJECT_T_API UILW_Modal : public UIL_UserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_Guide{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_OK{};

public:
	void NativeConstruct()override;
	void SetText(const FText& _GuideText);

private:
	UFUNCTION()
	void OnClicked();
	
};
