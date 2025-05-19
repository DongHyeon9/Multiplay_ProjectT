#pragma once

#include "InGame/Widget/IG_UserWidget.h"
#include "IGW_CharacterStatus.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class PROJECT_T_API UIGW_CharacterStatus : public UIG_UserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,category="UIGW_CharacterStatus",meta=(BindWidget,AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> txt_UserName{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGW_CharacterStatus", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UProgressBar> pb_CharacterHP{};

public:
	void NativeConstruct()override;
	void SetName(const FText& _NewName);
	
};
