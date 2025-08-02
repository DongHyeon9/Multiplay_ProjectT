#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "IGW_ResultPopup.generated.h"

class UButton;
class UVerticalBox;
class UIGW_ResultCard;

UCLASS()
class PROJECT_T_API UIGW_ResultPopup : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> btn_Home{};
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UVerticalBox> vb_ResultCards{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGW_ResultPopup", meta = (AllowPrivateAccess = true))
	TSubclassOf<UIGW_ResultCard> resultCardClass{};

public:
	void NativeConstruct()override;
	void InitResultPopup(const TArray<TObjectPtr<APlayerState>>& _ResultInfo);

	UFUNCTION()
	void OnClickButton();
};