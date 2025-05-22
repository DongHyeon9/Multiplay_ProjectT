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
	// 위젯에 표시될 캐릭터의 이름
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGW_CharacterStatus", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> txt_UserName{};

	// 현재 캐릭터의 HP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGW_CharacterStatus", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UProgressBar> pb_CharacterHP{};

public:
	void NativeConstruct()override;
	// 위젯의 캐릭터 명을 바꿔준다
	// @param _NewName 설정할 이름
	void SetName(const FText& _NewName);
	
};
