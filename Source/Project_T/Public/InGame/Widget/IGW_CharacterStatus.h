#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "IGW_CharacterStatus.generated.h"

class UTextBlock;
class UProgressBar;
class UIG_StatComponent;

UCLASS()
class PROJECT_T_API UIGW_CharacterStatus : public UUserWidget
{
	GENERATED_BODY()
public:

protected:

private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> txt_Name{};
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> pb_HPBar{};

	float maxHP{};

public:
	void InitCharacterStatus(UIG_StatComponent* _StatComp, const FString& _Name = TEXT(""));
	void SetName(const FString& _Name);
	void SetName(FText _Name);
	void SetMaxHP(float _MaxHP);

protected:

private:
	void OnChangeHP(float _Prev, float _Current);
};
