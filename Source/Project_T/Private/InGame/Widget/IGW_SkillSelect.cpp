#include "InGame/Widget/IGW_SkillSelect.h"
#include "Components/TileView.h"
#include "InGame/Widget/IGW_SkillCard.h"

void UIGW_SkillSelect::NativeConstruct()
{
	Super::NativeConstruct();
	
	FWidgetAnimationDynamicEvent disapperanceAnimDelegate{};
	disapperanceAnimDelegate.BindDynamic(this, &UIGW_SkillSelect::OnFinishedDisappearanceAnim);
	BindToAnimationFinished(disappearanceAnim, disapperanceAnimDelegate);
	tv_ItemList->OnItemSelectionChanged().AddUObject(this, &UIGW_SkillSelect::OnSelectedItem);
}

void UIGW_SkillSelect::InitializeSkillSelect(const TArray<UIG_SkillCardInfo*>& _SkillCardInfo)
{
	tv_ItemList->ClearListItems();
	for (auto cardInfo : _SkillCardInfo)
	{
		tv_ItemList->AddItem(cardInfo);
	}
}

void UIGW_SkillSelect::Appearance()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(appearanceAnim);
}

void UIGW_SkillSelect::Disappearance()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayAnimation(disappearanceAnim);
}

void UIGW_SkillSelect::OnSelectedItem(UObject* _Item)
{
	auto skillInfo{ Cast<UIG_SkillCardInfo>(_Item) };
	if (!skillInfo) return;
	selectedSkillName = skillInfo->GetSkillName();
	Disappearance();
}

void UIGW_SkillSelect::OnFinishedDisappearanceAnim()
{
	SetVisibility(ESlateVisibility::Hidden);
	onSelectedSkill.ExecuteIfBound(selectedSkillName);
}