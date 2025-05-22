#pragma once

#include <atomic>

#include "../Project_T.h"
#include "GameFramework/GameState.h"
#include "Containers/Ticker.h"
#include "IG_GameState.generated.h"

class AIGC_Enemy;
class AIG_PlayerController;

enum class E_CHARACTER_STATE : uint8;

UCLASS()
class PROJECT_T_API AIG_GameState : public AGameState
{
	GENERATED_BODY()
public:
	std::atomic<int32> compeletedPlayer{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	float gameTime{ 10.0f };

	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	float currentTime{};

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	bool bIsPlaying{};

	FTSTicker::FDelegateHandle gameTimerHandle{};

private:
	TQueue<AIGC_Enemy*> enemyPool{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	TSubclassOf<AIGC_Enemy> enemyClass{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	int32 objectPoolDefaultSize{ 100 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> difficultyCurve{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	float spawnInterval{ 1.0f };

	FTSTicker::FDelegateHandle spawnHandle{};

	float curveLength{};

public:
	AIG_GameState(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	AIGC_Enemy* GetEnemyInPool();

	void OnInitPlayer();
	void RequestStartGame();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	FORCEINLINE int32 GetTimer()const { return static_cast<int32>(currentTime); }

private:
	bool GameTimer(float _DeltaTime);
	void StartGame();
	void EndGame();

	void OnEnemyStateChange(AIGC_Enemy* _Enemy, E_CHARACTER_STATE _NewState);

	bool EnemySpawn(float _DeltaTime);
	void ExpandPool();
	void ActiveEnemy(AIGC_Enemy* _Enemy);
	AIGC_Enemy* SpawnEnemy();
};
