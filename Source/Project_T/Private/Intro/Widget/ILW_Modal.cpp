#include "Intro/Widget/ILW_Modal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UILW_Modal::NativeConstruct()
{
	Super::NativeConstruct();

	btn_OK->OnClicked.AddDynamic(this, &UILW_Modal::OnClicked);
}

void UILW_Modal::SetText(const FText& _GuideText)
{
	txt_Guide->SetText(_GuideText);
}

void UILW_Modal::OnClicked()
{
	RemoveFromParent();
}
