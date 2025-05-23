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
	// 플레이어 접속 상태를 동기화하기 위한 변수
	std::atomic<int32> compeletedPlayer{};

	// 인게임에 총 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	float gameTime{ 10.0f };

	// 현재 진행된 시간
	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	float currentTime{};

	// 현재 게임이 진행중인지 판별
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, category = "AIG_GameState", meta = (AllowPrivateAccess = true))
	bool bIsPlaying{};

	// 게임이 진행중일때 시간을 체크하는 타이머의 핸들
	FTSTicker::FDelegateHandle gameTimerHandle{};

private:
	// 소환될 적의 오브젝트 풀
	TQueue<AIGC_Enemy*> enemyPool{};

	// 소환될 적의 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	TSubclassOf<AIGC_Enemy> enemyClass{};

	// 오브젝트 풀의 기본 사이즈
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	int32 objectPoolDefaultSize{ 100 };

	//풀의 확장 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	float expansionRatio{ 1.5f };

	// 게임의 난이도를 시간에 따라 정하는 커브
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> difficultyCurve{};

	// 소환될 시간 간격
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameState Enemy Pool", meta = (AllowPrivateAccess = true))
	float spawnInterval{ 1.0f };

	// spawnInterval마다 소환시키기 위한 타이머의 핸들
	FTSTicker::FDelegateHandle spawnHandle{};

	// 커브의 총 길이
	float curveLength{};

public:
	AIG_GameState(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void EndPlay(EEndPlayReason::Type _Reason)override;
	// 오브젝트 풀에서 적을 가져온다
	// 만약 풀에 오브젝트가 없으면 풀을 늘린다
	// @return 풀에서 가져온 적(SetActive는 따로 호출해줘야된다)
	AIGC_Enemy* GetEnemyInPool();
	// 플레이어가 접속했다면 게임스테이트에 알려준다
	void OnInitPlayer();
	// 모든 플레이어가 시작 이벤트가 끝났으면 게임을 시작한다
	void RequestStartGame();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 현재 남은 시간을 반환한다
	// @return 남은시간
	FORCEINLINE int32 GetTimer()const { return static_cast<int32>(currentTime); }

private:
	// 진행된 게임의 시간을 측정
	// @param _DeltaTime 이전 프레임으로 부터 지난 시간
	// @return true면 다음에도 실행된다
	bool GameTimer(float _DeltaTime);
	// 게임을 시작한다
	void StartGame();
	// 게임을 종료한다
	void EndGame();
	// 적의 상태가 변경됐을때 호출되는 CallBack함수
	// @param _Enemy 상태가 변경된 적
	// @param _NewState 변경된 상태
	void OnEnemyStateChange(AIGC_Enemy* _Enemy, E_CHARACTER_STATE _NewState);
	// 일정 시간마다 적을 소환하는 함수
	// @param _DeltaTime 
	// @return true면 다음에도 실행된다
	bool EnemySpawn(float _DeltaTime);
	// 풀의 크기를 늘린다
	void ExpandPool();
	// 적을 특정 위치(플레이어의 카메라 밖)으로 보낸 뒤 활성화 한다
	// @param _Enemy 활성화 시킬 적
	void ActiveEnemy(AIGC_Enemy* _Enemy);
	// 적을 소환하고 초기화 작업을 진행한다
	// @return 소환한 적
	AIGC_Enemy* SpawnEnemy();
};
