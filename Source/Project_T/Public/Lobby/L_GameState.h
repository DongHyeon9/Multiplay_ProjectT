#pragma once

#include "../Project_T.h"
#include "GameFramework/GameState.h"
#include "L_GameState.generated.h"

UCLASS()
class PROJECT_T_API AL_GameState : public AGameState
{
	GENERATED_BODY()
public:
	FOnLoginPlayerDelegate onLoginPlayer{};
	FOnLogoutPlayerDelegate onLogoutPlayer{};

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AL_GameState", meta = (AllowPrivateAccess = true))
	TArray<APlayerController*> connectedPlayers{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AL_GameState", meta = (AllowPrivateAccess = true))
	int32 maxPlayer{};

public:
	void BeginPlay()override;

	UFUNCTION(Client, Reliable)
	void Client_Init(int32 _MaxPlayer, const TArray<APlayerController*>& _ConnectedPlayers);
	void Client_Init_Implementation(int32 _MaxPlayer, const TArray<APlayerController*>& _ConnectedPlayers);

	UFUNCTION(Client, Reliable)
	void Client_LoginPlayer(const APlayerController* _LoginPlayer);
	void Client_LoginPlayer_Implementation(const APlayerController* _LoginPlayer);

	UFUNCTION(Client, Reliable)
	void Client_LogoutPlayer(const APlayerController* _LoginPlayer);
	void Client_LogoutPlayer_Implementation(const APlayerController* _LoginPlayer);

public:
	FORCEINLINE const TArray<APlayerController*>& GetCurrentPlayers()const { return connectedPlayers; }
	FORCEINLINE int32 GetMaxPlayer()const { return maxPlayer; }
};
