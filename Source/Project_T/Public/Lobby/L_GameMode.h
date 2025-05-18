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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_GameMode", meta = (AllowPrivateAccess = true))
	int32 maxPlayer{ 3 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_GameMode", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UWorld> mainLevel{};

public:
	AL_GameMode(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void PreLogin(const FString& _Options, const FString& _Address, const FUniqueNetIdRepl& _UniqueId, FString& _ErrorMessage) override;
	void EndPlay(EEndPlayReason::Type _Reason)override;
	void OnLoginComplete();

public:
	FORCEINLINE int32 GetMaxPlayer()const { return maxPlayer; }

private:
	void OnCreateSessionComplete(FName _SessionName, bool _bWasSuccessful);
};
