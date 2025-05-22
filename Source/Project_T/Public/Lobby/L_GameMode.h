#pragma once

#include "../Project_T.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/GameMode.h"
#include "L_GameMode.generated.h"

UCLASS()
class PROJECT_T_API AL_GameMode : public AGameMode
{
	GENERATED_BODY()
private:
	// 게임이 진행될 메인 레벨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_GameMode", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UWorld> mainLevel{};

public:
	AL_GameMode(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	// 플레이어의 로그인을 처리한다
	// 만약 MAX_PLAYER_COUNT를 초과하는 플레이어가 입장했을시 입장을 거부한다
	void PreLogin(const FString& _Options, const FString& _Address, const FUniqueNetIdRepl& _UniqueId, FString& _ErrorMessage) override;
	void EndPlay(EEndPlayReason::Type _Reason)override;
	// 모든 플레이어가 접속하면 메인 레벨로 ServerTravel을 진행한다
	void OnLoginComplete();

private:
	void OnCreateSessionComplete(FName _SessionName, bool _bWasSuccessful);
};
