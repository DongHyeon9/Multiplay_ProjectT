#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "IGW_ResultCard.generated.h"

class UTextBlock;

UCLASS()
class PROJECT_T_API UIGW_ResultCard : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> txt_Score{};

public:
	void InitResultCard(TObjectPtr<APlayerState> _PlayerState);
	
};
