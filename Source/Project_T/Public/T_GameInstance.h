#pragma once

#include "../Project_T.h"
#include "Engine/GameInstance.h"
#include "T_GameInstance.generated.h"

UCLASS()
class PROJECT_T_API UT_GameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
	// 유저의 이름을 로컬에 저장한다
	UPROPERTY()
	FString userName{};

public:
	// 유저의 이름을 저장한다 GameInstance에 저장될땐 Prefix와 같이 저장된다
	// @param _UserName 저장할 유저의 이름
	void SetUserName(const FString& _UserName);
	// 현재 저장된 유저의 이름을 반환한다
	// @return 저장된 유저의 이름
	FORCEINLINE const FString& GetUserName()const { return userName; }
};
