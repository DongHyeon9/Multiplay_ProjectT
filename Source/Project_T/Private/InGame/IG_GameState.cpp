#include "InGame/IG_GameState.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/Component/IG_StatComponent.h"
#include "InGame/IG_GameMode.h"
#include "InGame/Enemy/IGC_Enemy.h"
#include "InGame/Component/PlayerController/IGM_Screen.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"

#if WITH_EDITOR
namespace DEBUG
{
	static int32 TOTAL_SPAWN_COUT{};
}
#endif

namespace INGAME_GAME_STATE
{
	static constexpr const TCHAR* ENEMY_CLASS_PATH{ TEXT("/Game/01_Blueprint/InGame/Enemy/BP_IGC_Enemy") };
	static constexpr const TCHAR* DIFFICULTY_PATH{ TEXT("/Game/07_GameData/C_Difficulty") };
}

AIG_GameState::AIG_GameState(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<AIGC_Enemy> IGC_ENEMY{ INGAME_GAME_STATE::ENEMY_CLASS_PATH };
	static ConstructorHelpers::FObjectFinder<UCurveFloat> C_DIFFICULTY{ INGAME_GAME_STATE::DIFFICULTY_PATH };

	if (IGC_ENEMY.Succeeded()) enemyClass = IGC_ENEMY.Class;
	if (C_DIFFICULTY.Succeeded()) difficultyCurve = C_DIFFICULTY.Object;

	compeletedPlayers.Reserve(MAX_PLAYER_COUNT);
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
	DEBUG::TOTAL_SPAWN_COUT = 0;
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
	SERVER_ONLY(nullptr);
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
	SERVER_ONLY(nullptr);
	//적을 스폰하고 초기화를 진행
	FTransform spawnTransform{};
	FActorSpawnParameters param{};

	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AIGC_Enemy* result{ GetWorld()->SpawnActor<AIGC_Enemy>(enemyClass, spawnTransform, param) };
	PTT_ERRER(result, nullptr);

	if (!result->onEnemyState.IsBoundToObject(this))
		result->onEnemyState.BindUObject(this, &AIG_GameState::OnEnemyStateChange);

	return result;
}

void AIG_GameState::OnEnterPlayer(const AIG_PlayerController* _PC)
{
	SERVER_ONLY();
	//시간 동기화를 위해
	//입장한 플레이들이 준비됐는지 확인한다
	compeletedPlayers.Emplace(_PC);
	PTT_LOG(Warning, TEXT("compeletedPlayer : %s"), *_PC->GetName());
	if (compeletedPlayers.Num() != MAX_PLAYER_COUNT) return;

	UWorld* world = GetWorld();
	checkf(world, TEXT("World Is Null"));
	compeletedPlayers.Empty();
	auto gm = world->GetAuthGameMode<AIG_GameMode>();
	checkf(gm, TEXT("AIG_GameMode Is Null"));
	gm->OnEnterPlayer();
	for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		//모든 플레이어가 입장했으면 시작 이벤트를 진행한다
		if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
		{
			pc->Client_StartEvent();
		}
	}
}

void AIG_GameState::RequestStartGame(const AIG_PlayerController* _PC)
{
	SERVER_ONLY();
	// 모든 플레이어가 시작 이벤트가 끝났으면
	// 동기화 후 게임을 시작한다
	compeletedPlayers.Emplace(_PC);
	PTT_LOG(Warning, TEXT("compeletedPlayer : %s"), *_PC->GetName());
	if (compeletedPlayers.Num() != MAX_PLAYER_COUNT) return;

	compeletedPlayers.Empty();
	StartGame();
}

bool AIG_GameState::Ticker_GameTimer(float _DeltaTime)
{
	SERVER_ONLY(false);

	--currentTime;
	PTT_LOG(Warning, TEXT("%.1f"), currentTime);
	bIsPlaying = currentTime > 0.0f && !FMath::IsNearlyZero(currentTime);
	if (!bIsPlaying)
	{
		EndGame();
	}
	return bIsPlaying;
}

bool AIG_GameState::Ticker_EnemySpawn(float _DeltaTime)
{
	SERVER_ONLY(false);

	if (!bIsPlaying) return false;

	const float timeInCurve{ currentTime / gameTime * curveLength };
	int32 spawnCount{ static_cast<int32>(difficultyCurve->GetFloatValue(timeInCurve)) };
	PTT_LOG(Warning, TEXT("Spawn Count : %d"), spawnCount);

	for (int32 i = 0; i < spawnCount; ++i)
	{
		auto monster{ GetEnemyInPool() };
		ActiveEnemy(monster);
	}

#if WITH_EDITOR
	DEBUG::TOTAL_SPAWN_COUT += spawnCount;
	PRINTSTR(-1, 1.0f, FColor::Red, TEXT("Total Spawned : %d"), DEBUG::TOTAL_SPAWN_COUT);
	PRINTSTR(-1, 1.0f, FColor::Green, TEXT("PoolSize : %d"), objectPoolDefaultSize);
#endif

	return true;
}

void AIG_GameState::OnEnemyStateChange(AIGC_Enemy* _Enemy, E_CHARACTER_STATE _NewState)
{
	SERVER_ONLY();

	if (_NewState == E_CHARACTER_STATE::DISABLE)
		enemyPool.Enqueue(_Enemy);
}

void AIG_GameState::StartGame()
{
	SERVER_ONLY();

	currentTime = gameTime;
#if !WITH_EDITOR
	for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
		{
			// 서버에서 플레이어 초기화
			pc->Client_StartGame();
		}
	}
#endif

	//커브의 길이를 저장한다
	float curveMinValue{};
	float curveMaxValue{};
	difficultyCurve->GetValueRange(curveMinValue, curveMaxValue);
	curveLength = curveMaxValue - curveMinValue;

	// 타이머 측정과 스폰을 시작한다
	// 타이머는 최적화를 위해 1초에 한번씩 실행한다
	// 게임 시간이 시간 혹은 일 단위가 되지 않기 때문에 약간의 시간차이는 무시하고 성능 향상 기대
	gameTimerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::Ticker_GameTimer), 1.0f);
	spawnHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::Ticker_EnemySpawn), spawnInterval);

	PTT_LOG(Warning, TEXT("Game Start!"));
}

void AIG_GameState::EndGame()
{
	SERVER_ONLY();

	// 핸들을 Release 해준다
	FTSTicker& ticker{ FTSTicker::GetCoreTicker() };
	if (gameTimerHandle.IsValid()) ticker.RemoveTicker(gameTimerHandle), gameTimerHandle.Reset();
	if (spawnHandle.IsValid()) ticker.RemoveTicker(spawnHandle), spawnHandle.Reset();

	// 게임 시간이 끝났으면 모든 적을 제거한다
	TActorRange<AIGC_Enemy> enemies{ GetWorld() };
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
			pc->EndGame();
		}
	}
	PTT_LOG(Warning, TEXT("Game End!"));
}

void AIG_GameState::ExpandPool()
{
	SERVER_ONLY();

	//일정 배율만큼 풀사이즈를 늘려준다
	int32 newSize{ static_cast<int32>(objectPoolDefaultSize * expansionRatio) };
	PTT_LOG(Warning, TEXT("New Pool Size : %d"), newSize);
	for (int32 i = objectPoolDefaultSize; i < newSize; ++i)
	{
		auto enemy{ SpawnEnemy() };
		enemyPool.Enqueue(enemy);
	}

	objectPoolDefaultSize = newSize;
}

void AIG_GameState::ActiveEnemy(AIGC_Enemy* _Enemy)
{
	SERVER_ONLY();

	UWorld* world{ GetWorld() };
	TArray<TPair<FVector, float>> playerCameraInfo{};
	playerCameraInfo.Reserve(world->GetNumPlayerControllers());
	const float capsuleRadius{ _Enemy->GetRootComp()->GetScaledCapsuleRadius() };
	FHitResult outResult{};

	for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		auto pc = Cast<AIG_PlayerController>(pIter->Get());
		if (!pc) continue;

		TPair<FVector, float> screenInfo{ pc->GetPawn()->GetActorLocation(), pc->GetScreenManager()->GetScreenRadius() };
		screenInfo.Value += capsuleRadius;
		playerCameraInfo.Emplace(screenInfo);
	}

	//영점으로부터 가까운 순서대로 정렬
	Algo::Sort(playerCameraInfo, [](const TPair<FVector, float>& _Lhs, const TPair<FVector, float>& _Rhs) {return _Lhs.Key.SquaredLength() > _Rhs.Key.SquaredLength(); });

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
	spawnLocation.Z += capsuleRadius;

	_Enemy->SetActorLocation(spawnLocation);
	_Enemy->ActiveEnemy();
}

void AIG_GameState::OnGameOverPlayer(const AIG_PlayerController* _PC)
{
	compeletedPlayers.Emplace(_PC);
	if (compeletedPlayers.Num() != MAX_PLAYER_COUNT) return;
	EndGame();
}

void AIG_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_GameState, currentTime);
}
