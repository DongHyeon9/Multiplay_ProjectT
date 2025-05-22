#include "InGame/IG_GameState.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/IG_GameMode.h"
#include "Net/UnrealNetwork.h"
#include "InGame/Enemy/IGC_Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h"

AIG_GameState::AIG_GameState(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<AIGC_Enemy> IGC_ENEMY(TEXT("/Game/01_Blueprint/InGame/Enemy/BP_IGC_Enemy"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> C_DIFFICULTY(TEXT("/Game/07_GameData/C_Difficulty"));

	if (IGC_ENEMY.Succeeded()) enemyClass = IGC_ENEMY.Class;
	if (C_DIFFICULTY.Succeeded()) difficultyCurve = C_DIFFICULTY.Object;
}

void AIG_GameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		for (int32 i = 0; i < objectPoolDefaultSize; ++i)
		{
			PTT_LOG(Warning, TEXT("Spawn : %d"), i + 1);
			auto enemy{ SpawnEnemy() };
			enemyPool.Enqueue(enemy);
		}
	}

#if WITH_EDITOR
	StartGame();
#endif
}

void AIG_GameState::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	FTSTicker& ticker{ FTSTicker::GetCoreTicker() };
	if (gameTimerHandle.IsValid()) ticker.RemoveTicker(gameTimerHandle), gameTimerHandle.Reset();
	if (spawnHandle.IsValid()) ticker.RemoveTicker(spawnHandle), spawnHandle.Reset();
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

	const float timeInCurve{ currentTime / gameTime * curveLength };
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
	while (!enemyPool.IsEmpty())
	{
		AIGC_Enemy* monster{};
		enemyPool.Dequeue(monster);
		monster->Destroy();
	}

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
	int32 newSize{ objectPoolDefaultSize * 1.5 };
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

		int32 sizeX{}, sizeY{};
		pc->GetViewportSize(sizeX, sizeY);

		//중앙 스크린의 월드좌표 구하기
		FVector worldLocationM{}, worldDirectionM{};
		pc->DeprojectScreenPositionToWorld(sizeX * 0.5, sizeY * 0.5, worldLocationM, worldDirectionM);
		FVector targetLocation{ worldDirectionM * traceDistance + worldLocationM };
		FHitResult outResult{};
		FCollisionObjectQueryParams param{};
		if (world->LineTraceSingleByObjectType(
			outResult,
			worldLocationM,
			targetLocation,
			UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
		{
			worldLocationM = outResult.ImpactPoint;
		}

		//좌측 상단 스크린의 월드좌표 구하기
		FVector worldLocationTL{}, worldDirectionTL{};
		pc->DeprojectScreenPositionToWorld(0, 0, worldLocationTL, worldDirectionTL);
		targetLocation = worldDirectionTL * traceDistance + worldLocationTL;
		if (world->LineTraceSingleByObjectType(
			outResult,
			worldLocationTL,
			targetLocation,
			UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
		{
			worldLocationTL = outResult.ImpactPoint;
		}
		
		//현재 스크린을 완전히 덮는 구의 반지름 구하기
		float radius{ static_cast<float>((worldLocationM - worldLocationTL).Length()) };
		playerCameraInfo.Emplace(worldLocationM, abs(radius) + capsuleRadius);
	}

	playerCameraInfo.Sort([](TPair<FVector, float> _Lhs, TPair<FVector, float> _Rhs) {
		return _Lhs.Key.SquaredLength() > _Rhs.Key.SquaredLength();
		});

	const float rangeMin{ -100.0f };
	const float rangeMax{ 100.0f };
	FVector randomDir{ FMath::FRandRange(rangeMin, rangeMax),FMath::FRandRange(rangeMin, rangeMax) ,0.0f };
	randomDir.Normalize();

	FVector spawnLocation{};

	// 배열을 순회하며
	// 현재 위치가 카메라의 범위에 포함되어 있으면
	// 그 거리만큼 randomDir방향으로 밀어낸다
	for (const auto& cameraInfo : playerCameraInfo)
	{
		float distance{};
		do
		{
			distance = static_cast<float>(abs((spawnLocation - cameraInfo.Key).Length()));
			const float moveDistance{ cameraInfo.Value - distance };
			spawnLocation += randomDir * moveDistance;
		} while (distance < cameraInfo.Value);
	}

	spawnLocation.Z += _Enemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	_Enemy->SetActorLocation(spawnLocation);
	_Enemy->SetActive(true);
}

void AIG_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_GameState, currentTime);
}
