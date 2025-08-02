#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "IGW_SkillCard.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PROJECT_T_API UIGW_SkillCard : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

protected:

private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> img_SkillIcon{};
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> txt_Name{};
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> txt_Desc{};
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> txt_SkillLevel{};

public:
	void NativeOnListItemObjectSet(UObject* _Obj) override;

protected:

private:

};

UCLASS()
class PROJECT_T_API UIG_SkillCardInfo : public UObject
{
	GENERATED_BODY()
public:

protected:

private:
	FName skillName{};
	FText displayName{};
	FText description{};
	UTexture2D* skillIcon{};
	int32 skillLevel{};

public:
	void InitSkillItem(const FSkillInfoParam& _Param);

	FORCEINLINE FName GetSkillName()const { return skillName; }
	FORCEINLINE FText GetDisplayName()const { return displayName; }
	FORCEINLINE FText GetDescription()const { return description; }
	FORCEINLINE UTexture2D* GetSkillIcon()const { return skillIcon; }
	FORCEINLINE int32 GetSkillLevel()const { return skillLevel; }

protected:

private:

};
