#include "InGame/Widget/IGW_CharacterStatus.h"
#include "InGame/Component/IG_StatComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UIGW_CharacterStatus::InitCharacterStatus(UIG_StatComponent* _StatComp, const FString& _Name)
{
	txt_Name->SetText(FText::FromString(_Name));
	if (!_StatComp->onHPChange.IsBoundToObject(this))
		_StatComp->onHPChange.AddUObject(this, &UIGW_CharacterStatus::OnChangeHP);
	maxHP = _StatComp->GetCharacterData()->maxHP;
	pb_HPBar->SetPercent(1.0f);
}

void UIGW_CharacterStatus::SetName(const FString& _Name)
{
	txt_Name->SetText(FText::FromString(_Name));
}

void UIGW_CharacterStatus::SetName(FText _Name)
{
	txt_Name->SetText(_Name);
}

void UIGW_CharacterStatus::SetMaxHP(float _MaxHP)
{
	maxHP = _MaxHP;
}

void UIGW_CharacterStatus::OnChangeHP(float _Prev, float _Current)
{
	pb_HPBar->SetPercent(_Current / maxHP);
}
