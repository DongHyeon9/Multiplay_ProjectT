#pragma once

#include "../Project_T.h"
#include "Engine/GameInstance.h"
#include "T_GameInstance.generated.h"

UCLASS()
class PROJECT_T_API UT_GameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FString userName{};

public:
	void SetUserName(const FString& _UserName);
	FORCEINLINE const FString& GetUserName()const { return userName; }
};
