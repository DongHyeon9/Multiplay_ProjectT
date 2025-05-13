#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "IL_UserWidget.generated.h"

UCLASS()
class PROJECT_T_API UIL_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct()override;
	
};
