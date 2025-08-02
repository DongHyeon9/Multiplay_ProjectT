#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "IGW_SkillSelect.generated.h"

class UTileView;
class UIG_SkillCardInfo;

UCLASS()
class PROJECT_T_API UIGW_SkillSelect : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnSelectedSkillDelegate onSelectedSkill{};
protected:

private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTileView> tv_ItemList{};

	UPROPERTY(Transient, BlueprintReadOnly, category = "UIGW_SkillSelect", meta = (BindWidgetAnim, AllowPrivateAccess = true))
	UWidgetAnimation* appearanceAnim{};

	UPROPERTY(Transient, BlueprintReadOnly, category = "UIGW_SkillSelect", meta = (BindWidgetAnim, AllowPrivateAccess = true))
	UWidgetAnimation* disappearanceAnim{};

	FName selectedSkillName{};

public:
	void NativeConstruct()override;
	void InitializeSkillSelect(const TArray<UIG_SkillCardInfo*>& _SkillCardInfo);
	void Appearance();
	void Disappearance();

protected:

private:
	UFUNCTION()
	void OnSelectedItem(UObject* _Item);

	UFUNCTION()
	void OnFinishedDisappearanceAnim();
};
