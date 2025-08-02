#include "InGame/Widget/IGW_ResultPopup.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "InGame/Widget/IGW_ResultCard.h"
#include "Kismet/GameplayStatics.h"

void UIGW_ResultPopup::NativeConstruct()
{
	Super::NativeConstruct();

	btn_Home->OnClicked.AddDynamic(this, &UIGW_ResultPopup::OnClickButton);
}

void UIGW_ResultPopup::InitResultPopup(const TArray<TObjectPtr<APlayerState>>& _ResultInfo)
{
	vb_ResultCards->ClearChildren();
	for (auto ps : _ResultInfo)
	{
		// 카드 셋팅
		auto resultCard{ CreateWidget<UIGW_ResultCard>(GetOwningPlayer(), resultCardClass) };
		resultCard->InitResultCard(ps);
		if (auto resultCardSlot{ Cast<UVerticalBoxSlot>(vb_ResultCards->AddChild(resultCard)) })
		{
			resultCardSlot->SetSize(ESlateSizeRule::Fill);
			resultCardSlot->SetPadding({
				0.0f,	//Left
				0.0f,	//Top
				0.0f,	//Right
				50.0f	//Bottom
				});
		}
	}
}

void UIGW_ResultPopup::OnClickButton()
{
	// 홈으로 돌아가기
	UGameplayStatics::OpenLevel(GetOwningPlayer(), INTRO_LEVEL_NAME);
}
