#pragma once

#include "../Project_T.h"
#include "GameFramework/GameState.h"
#include <atomic>
#include "Containers/Ticker.h"
#include "IG_GameState.generated.h"

UCLASS()
class PROJECT_T_API AIG_GameState : public AGameState
{
	GENERATED_BODY()
public:
	std::atomic<int32> compeletedPlayer{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	float gameTime{ 300.0f };

	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	float currentTime{};

	FTSTicker::FDelegateHandle gameTimerHandle{};

public:
	void OnCompletePlayer();
	void StartGame();
	void EndGame();
	bool GameTimer(float _DeltaTime);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
