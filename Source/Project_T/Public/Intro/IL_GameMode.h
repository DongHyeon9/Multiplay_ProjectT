#pragma once

#include "../Project_T.h"
#include "GameFramework/GameMode.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "IL_GameMode.generated.h"

DECLARE_DELEGATE(FOnJoinFailDelegate);

UCLASS()
class PROJECT_T_API AIL_GameMode : public AGameMode
{
	GENERATED_BODY()
	
private:
	// Online Subsystem 세션 인터페이스
	IOnlineSessionPtr sessionInterface{};

	// 세션 탐색 설정
	TSharedPtr<FOnlineSessionSearch> sessionSearch{};

	// 델리게이트 핸들
	TArray<FDelegateHandle> findSessionsCompleteHadles{};

	FOnJoinFailDelegate onJoinFail{};

public:
	AIL_GameMode(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void EndPlay(EEndPlayReason::Type _Reason)override;

	void MatchCancel();
	bool MatchStart(FOnFindSessionsCompleteDelegate _Find = {}, FOnJoinFailDelegate _JoinFail = {});

private:
	UFUNCTION()
	void OnFindSessionsComplete(bool _bWasSuccessful);

	void OnJoinSessionComplete(FName _SessionName, EOnJoinSessionCompleteResult::Type _Result);
};
