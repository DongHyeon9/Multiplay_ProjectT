#include "Intro/Widget/ILW_Modal.h"
#include "Components/TextBlock.h"

void UILW_Modal::NativeConstruct()
{
	Super::NativeConstruct();

	btn_OK->OnClicked.AddDynamic(this, &UILW_Modal::OnClicked);
}

void UILW_Modal::SetText(const FText& _GuideText, const FText& _ButtonText)
{
	txt_Guide->SetText(_GuideText);
	txt_ButtonText->SetText(_ButtonText);
}

void UILW_Modal::OnClicked()
{
	onClicked.Broadcast();
	RemoveFromParent();
}
