#include "InGame/Widget/IGW_CharacterStatus.h"
#include "Components/TextBlock.h"

void UIGW_CharacterStatus::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIGW_CharacterStatus::SetName(const FText& _NewName)
{
	txt_UserName->SetText(_NewName);
}
