#include "InGame/IG_GameState.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/IG_GameMode.h"
#include "Net/UnrealNetwork.h"
#include "InGame/Enemy/IGC_Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"

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
		//서버에서 풀사이즈만큼 오브젝트를 생성한다
		for (int32 i = 0; i < objectPoolDefaultSize; ++i)
		{
			PTT_LOG(Warning, TEXT("Spawn : %d"), i + 1);
			auto enemy{ SpawnEnemy() };
			enemyPool.Enqueue(enemy);
		}
	}

	//에디터에서 실행시 디버깅을 위해 게임을 바로 시작한다
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
	//풀이 비어있으면 확장한다
	if (enemyPool.IsEmpty())
		ExpandPool();

	//풀에서 적을 가져와 생성한다
	AIGC_Enemy* monster{};
	enemyPool.Dequeue(monster);
	return monster;
}

AIGC_Enemy* AIG_GameState::SpawnEnemy()
{
	//적을 스폰하고 초기화를 진행
	FTransform spawnTransform{};
	FActorSpawnParameters param{};

	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AIGC_Enemy* result{ GetWorld()->SpawnActor<AIGC_Enemy>(enemyClass, spawnTransform, param) };
	result->InitEnemy();
	result->onEnemyState.BindUObject(this, &AIG_GameState::OnEnemyStateChange);

	return result;
}

void AIG_GameState::OnInitPlayer()
{
	//시간 동기화를 위해
	//입장한 플레이들이 준비됐는지 확인한다
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
			//모든 플레이어가 입장했으면 시작 이벤트를 진행한다
			pc->Client_StartEvent();
		}
	}
}

void AIG_GameState::RequestStartGame()
{
	// 모든 플레이어가 시작 이벤트가 끝났으면
	// 동기화 후 게임을 시작한다
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

	//커브의 길이를 저장한다
	float curveMinValue{};
	float curveMaxValue{};
	difficultyCurve->GetValueRange(curveMinValue, curveMaxValue);
	curveLength = curveMaxValue - curveMinValue;

	//타이머 측정과 스폰을 시작한다
	//타이머는 최적화를 위해 1초에 한번씩 실행한다
	gameTimerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::GameTimer), 1.0f);
	spawnHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::EnemySpawn), spawnInterval);

	PTT_LOG(Warning, TEXT("Game Start!"));
}

void AIG_GameState::EndGame()
{
	//게임 시간이 끝났으면 모든 적을 제거한다
	TActorRange<AIGC_Enemy> enemies(GetWorld());
	for (auto enemy : enemies)
	{
		enemy->Destroy();
	}
	enemyPool.Empty();

	//종료 이벤트를 실행한다
	for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
		{
			pc->Client_EndEvent();
		}
	}

	PTT_LOG(Warning, TEXT("Game End!"));
}

void AIG_GameState::ExpandPool()
{
	//일정 배율만큼 풀사이즈를 늘려준다
	int32 newSize{ static_cast<int32>(objectPoolDefaultSize * expansionRatio) };
	PTT_LOG(Warning, TEXT("New Pool Size : %d"), newSize);
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
	FHitResult outResult{};

	for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		auto pc = Cast<AIG_PlayerController>(pIter->Get());
		if (!pc) continue;

		TPair<FVector, float> screenInfo{ pc->GetScreenInfo() };
		screenInfo.Value += capsuleRadius;
		PTT_LOG(Warning, TEXT("%s Info : %s / %.4f"), *pc->GetName(), *screenInfo.Key.ToString(), screenInfo.Value);
		playerCameraInfo.Emplace(screenInfo);
	}

	//영점으로부터 가까운 순서대로 정렬
	playerCameraInfo.Sort([](TPair<FVector, float> _Lhs, TPair<FVector, float> _Rhs) {
		return _Lhs.Key.SquaredLength() > _Rhs.Key.SquaredLength();
		});

	//랜덤한 방향을 결정
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

	//소환될 Z값을 조정한다
	FVector startLoc{ spawnLocation };
	startLoc.Z -= 100.0f;
	FVector endLoc{ spawnLocation };
	endLoc.Z += 100.0f;
	if (world->LineTraceSingleByObjectType(
		outResult, 
		startLoc, 
		endLoc, 
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
	{
		spawnLocation = outResult.ImpactPoint;
	}
	spawnLocation.Z += _Enemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	_Enemy->SetActorLocation(spawnLocation);
	PTT_LOG(Warning, TEXT("Enemy Location : %s"), *spawnLocation.ToString());
	_Enemy->SetActive(true);
}

void AIG_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_GameState, currentTime);
}
