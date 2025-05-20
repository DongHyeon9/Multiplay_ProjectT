#include "InGame/IG_GameState.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/IG_GameMode.h"
#include "Net/UnrealNetwork.h"
#include "InGame/Enemy/IGC_Enemy.h"
#include "GameFramework/PlayerState.h"

void AIG_GameState::BeginPlay()
{
	Super::BeginPlay();
	
	for (int32 i = 0; i < objectPoolDefaultSize; ++i)
	{
		auto enemy{ SpawnEnemy() };
		enemyPool.Enqueue(enemy);
	}
#if WITH_EDITOR
	StartGame();
#endif
}

AIGC_Enemy* AIG_GameState::GetEnemyInPool()
{
	if (enemyPool.IsEmpty())
		ExpandPool();

	AIGC_Enemy* monster{};
	enemyPool.Dequeue(monster);
	return monster;
}

AIGC_Enemy* AIG_GameState::SpawnEnemy()
{
	FTransform spawnTransform{};
	AIGC_Enemy* result{ GetWorld()->SpawnActorDeferred<AIGC_Enemy>(enemyClass, spawnTransform, this) };
	result->InitEnemy();
	result->FinishSpawning(spawnTransform);
	result->onEnemyState.BindUObject(this, &AIG_GameState::OnEnemyStateChange);

	return result;
}

void AIG_GameState::OnCompletePlayer()
{
	++compeletedPlayer;
	PTT_LOG(Warning, TEXT("compeletedPlayer : %d"), compeletedPlayer.load());
	if (compeletedPlayer == MAX_PLAYER_COUNT)
	{
		compeletedPlayer = 0;
		UWorld* world = GetWorld();
		check(world);

		auto gm = world->GetAuthGameMode<AIG_GameMode>();
		check(gm);
		gm->OnCompleteAllPlayer();

		for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
		{
			if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
			{
				pc->Client_StartEvent();
			}
		}
	}
}

void AIG_GameState::RequestStartGame()
{
	++compeletedPlayer;
	PTT_LOG(Warning, TEXT("compeletedPlayer : %d"), compeletedPlayer.load());
	if (compeletedPlayer == MAX_PLAYER_COUNT)
	{
		compeletedPlayer = 0;

		StartGame();
	}
}

bool AIG_GameState::GameTimer(float _DeltaTime)
{
	--currentTime;
	PTT_LOG(Warning, TEXT("%.1f"), currentTime);
	bIsPlaying = currentTime - KINDA_SMALL_NUMBER <= 0.0f;
	if (bIsPlaying)
	{
		EndGame();
	}
	return !bIsPlaying;
}

bool AIG_GameState::EnemySpawn(float _DeltaTime)
{
	if (bIsPlaying) return false;

	auto monster{ GetEnemyInPool() };
	ActiveEnemy(monster);

	return true;
}

void AIG_GameState::OnEnemyStateChange(AIGC_Enemy* _Enemy, E_CHARACTER_STATE _NewState)
{
	_Enemy->SetActive(false);
	enemyPool.Enqueue(_Enemy);
}

void AIG_GameState::StartGame()
{
	currentTime = gameTime;

	for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
		{
			pc->Client_StartGame();
		}
	}

	gameTimerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::GameTimer), 1.0f);
	spawnHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::EnemySpawn));
}

void AIG_GameState::EndGame()
{
	for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
		{
			pc->Client_EndEvent();
		}
	}
}

void AIG_GameState::ExpandPool()
{
	int32 newSize = objectPoolDefaultSize * 1.5;
	for (int32 i = objectPoolDefaultSize; i < newSize; ++i)
	{
		auto enemy{ SpawnEnemy() };
		enemyPool.Enqueue(enemy);
	}
}

void AIG_GameState::ActiveEnemy(AIGC_Enemy* _Enemy)
{
	// TODO
	// 적 위치 선정

	_Enemy->SetActive(true);
}

void AIG_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_GameState, currentTime);
}
