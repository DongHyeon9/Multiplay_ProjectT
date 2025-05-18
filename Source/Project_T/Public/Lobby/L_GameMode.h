#pragma once

#include "../Project_T.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/GameMode.h"
#include "L_GameMode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnUpdateMatchStateDelegate);

UCLASS()
class PROJECT_T_API AL_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	FOnUpdateMatchStateDelegate onUpdateMatchState{};

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_GameMode", meta = (AllowPrivateAccess = true))
	int32 maxPlayer{ 4 };

public:
	void BeginPlay()override;
	void PostLogin(APlayerController* _NewPlayer)override;
	void Logout(AController* _Controller)override;
	void EndPlay(EEndPlayReason::Type _Reason)override;

public:
	FORCEINLINE int32 GetMaxPlayer()const { return maxPlayer; }

private:
	void OnCreateSessionComplete(FName _SessionName, bool _bWasSuccessful);
};
