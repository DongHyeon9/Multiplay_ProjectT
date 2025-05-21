#include "InGame/IG_GameState.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/IG_GameMode.h"
#include "Net/UnrealNetwork.h"
#include "InGame/Enemy/IGC_Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

void AIG_GameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		for (int32 i = 0; i < objectPoolDefaultSize; ++i)
		{
			PTT_LOG(Warning, TEXT("Spawn : %d"), i);
			auto enemy{ SpawnEnemy() };
			enemyPool.Enqueue(enemy);
		}
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
	AIGC_Enemy* result{ GetWorld()->SpawnActorDeferred<AIGC_Enemy>(enemyClass, spawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn) };
	result->InitEnemy();
	result->FinishSpawning(spawnTransform);
	result->onEnemyState.BindUObject(this, &AIG_GameState::OnEnemyStateChange);

	return result;
}

void AIG_GameState::OnInitPlayer()
{
	++compeletedPlayer;
	PTT_LOG(Warning, TEXT("compeletedPlayer : %d"), compeletedPlayer.load());
	if (compeletedPlayer != MAX_PLAYER_COUNT) return;

	UWorld* world = GetWorld();
	check(world);
	compeletedPlayer = 0;
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

	const float timeInCurve{ curveLength / gameTime };
	int32 spawnCount{ static_cast<int32>(difficultyCurve->GetFloatValue(timeInCurve)) };
	PTT_LOG(Warning, TEXT("Spawn Count : %d"), spawnCount);

	for (int32 i = 0; i < spawnCount; ++i)
	{
		auto monster{ GetEnemyInPool() };
		ActiveEnemy(monster);
	}

	return true;
}

void AIG_GameState::OnEnemyStateChange(AIGC_Enemy* _Enemy, E_CHARACTER_STATE _NewState)
{
	if (_NewState == E_CHARACTER_STATE::DISABLE)
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
	float curveMinValue{};
	float curveMaxValue{};
	difficultyCurve->GetValueRange(curveMinValue, curveMaxValue);
	curveLength = curveMaxValue - curveMinValue;
	gameTimerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::GameTimer), 1.0f);
	spawnHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::EnemySpawn), spawnInterval);
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
	UWorld* world{ GetWorld() };
	TArray<TPair<FVector, float>> playerCameraInfo{};
	playerCameraInfo.Reserve(world->GetNumPlayerControllers());
	const float capsuleRadius{ _Enemy->GetCapsuleComponent()->GetScaledCapsuleRadius() };
	for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		auto pc = Cast<AIG_PlayerController>(pIter->Get());
		if (!pc) continue;
		const float traceDistance{ 10000.0f };

		//좌측 상단 스크린의 월드좌표 구하기
		FVector worldLocationTL{}, worldDirectionTL{};
		pc->DeprojectScreenPositionToWorld(0, 0, worldLocationTL, worldDirectionTL);
		FVector targetLocation{ worldDirectionTL * traceDistance + worldLocationTL };
		FHitResult outResult{};
		FCollisionObjectQueryParams param{};
		if (world->LineTraceSingleByObjectType(
			outResult,
			worldLocationTL,
			targetLocation,
			UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
		{
			worldLocationTL = outResult.ImpactPoint;
		}

		//우측 하단 스크린의 월드좌표 구하기
		int32 sizeX{}, sizeY{};
		pc->GetViewportSize(sizeX, sizeY);
		FVector worldLocationBR{}, worldDirectionBR{};
		pc->DeprojectScreenPositionToWorld(sizeX, sizeY, worldLocationBR, worldDirectionBR);
		targetLocation = worldDirectionBR * traceDistance + worldLocationBR;
		if (world->LineTraceSingleByObjectType(
			outResult,
			worldDirectionBR,
			targetLocation,
			UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
		{
			worldLocationBR = outResult.ImpactPoint;
		}

		//중앙 좌표와 거리 구하기
		FVector center{ (worldLocationTL + worldLocationBR) * 0.5f };
		float radius{ static_cast<float>((center - worldLocationTL).Length()) };
		playerCameraInfo.Emplace(center, abs(radius) + capsuleRadius);
	}

	const float rangeMin{ -100.0f };
	const float rangeMax{ 100.0f };
	FVector randomDir{ FMath::FRandRange(rangeMin,rangeMax),FMath::FRandRange(rangeMin,rangeMax) ,0.0f };
	randomDir.Normalize();

	FVector spawnLocation{};

	// 배열을 순회하며
	// 현재 위치가 카메라의 범위에 포함되어 있으면
	// 그 거리만큼 randomDir방향으로 밀어낸다
	for (const auto& cameraInfo : playerCameraInfo)
	{
		const float distance{ static_cast<float>(abs((spawnLocation - cameraInfo.Key).Length())) };
		if (distance >= cameraInfo.Value) continue;
		const float moveDistance{ cameraInfo.Value - distance };
		spawnLocation += randomDir * moveDistance;
	}

	spawnLocation.Z = _Enemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	PTT_LOG(Warning, TEXT("%s SpawnLocation : %s"), *_Enemy->GetName(), *spawnLocation.ToString());
	_Enemy->SetActorLocation(spawnLocation);
	_Enemy->SetActive(true);
}

void AIG_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_GameState, currentTime);
}
