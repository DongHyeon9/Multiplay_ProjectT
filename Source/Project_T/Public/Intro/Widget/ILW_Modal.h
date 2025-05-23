#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ILW_Modal.generated.h"

class UTextBlock;

UCLASS()
class PROJECT_T_API UILW_Modal : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnButtonClickedEvent onClicked{};

private:
	// 텍스트가 표시될 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_Guide{};

	// 확인, 취소 버튼 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_ButtonText{};

	// 확인, 취소 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Modal Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_OK{};

public:
	void NativeConstruct()override;
	// 모달위젯의 텍스트를 설정
	// @param _GuideText 표시할 텍스트
	// @param _ButtonText 버튼에 표시할 텍스트
	void SetText(const FText& _GuideText, const FText& _ButtonText);

private:
	UFUNCTION()
	void OnClicked();
	
};
