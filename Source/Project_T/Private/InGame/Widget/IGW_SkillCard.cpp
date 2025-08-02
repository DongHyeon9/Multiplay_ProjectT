#include "InGame/Widget/IGW_SkillCard.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UIGW_SkillCard::NativeOnListItemObjectSet(UObject* _Obj)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(_Obj);

	auto skillCardInfo{ Cast<UIG_SkillCardInfo>(_Obj) };
	check(skillCardInfo);

	txt_Name->SetText(skillCardInfo->GetDisplayName());
	img_SkillIcon->SetBrushFromTexture(skillCardInfo->GetSkillIcon());
	txt_Desc->SetText(skillCardInfo->GetDescription());
	FString skillLevelText{};

	for (int32 i = 0; i < SKILL_MAX_LEVEL; ++i)
	{
		skillLevelText += 
			i >= skillCardInfo->GetSkillLevel() ? 
			TEXT("☆") : 
			TEXT("★");
	}

	txt_SkillLevel->SetText(FText::FromString(skillLevelText));
}

void UIG_SkillCardInfo::InitSkillItem(const FSkillInfoParam& _Param)
{
	skillName = _Param.skillName;
	displayName = _Param.displayName;
	description = _Param.description;
	skillIcon = _Param.skillIcon;
	skillLevel = _Param.skillLevel;
}
