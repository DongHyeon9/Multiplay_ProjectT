#pragma once

#include "../Project_T.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/GameMode.h"
#include "L_GameMode.generated.h"

UCLASS()
class PROJECT_T_API AL_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	FOnLoginPlayerDelegate onLoginPlayer{};
	FOnLogoutPlayerDelegate onLogoutPlayer{};

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_GameMode", meta = (AllowPrivateAccess = true))
	int32 maxPlayer{ 4 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AL_GameMode", meta = (AllowPrivateAccess = true))
	TArray<APlayerController*> connectedPlayers{};

public:
	void BeginPlay()override;
	void PostLogin(APlayerController* _NewPlayer) override;
	void Logout(AController* _Exiting) override;

public:
	FORCEINLINE const TArray<APlayerController*>& GetCurrentPlayers()const { return connectedPlayers; }
	FORCEINLINE int32 GetMaxPlayer()const { return maxPlayer; }

private:
	void OnCreateSessionComplete(FName _SessionName, bool _bWasSuccessful);
};
