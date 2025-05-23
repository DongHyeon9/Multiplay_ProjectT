#pragma once

#include "../Project_T.h"
#include "InGame/IG_CharacterBase.h"
#include "Containers/Ticker.h"
#include "IGC_Enemy.generated.h"

class AIGC_Player;

enum class E_CHARACTER_STATE : uint8;

DECLARE_DELEGATE_TwoParams(FOnEnemyStateDelegate, AIGC_Enemy*, E_CHARACTER_STATE);

UCLASS()
class PROJECT_T_API AIGC_Enemy : public AIG_CharacterBase
{
	GENERATED_BODY()
public:
	// 상태가 변경될 때 호출되는 델리게이트
	FOnEnemyStateDelegate onEnemyState{};

private:
	//몬스터가 죽었을때 비활성화되는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGC_Enemy", meta = (AllowPrivateAccess = true))
	float disableDelay{ 3.0f };

	//비활성화 타이머 핸들
	FTimerHandle disableHandle{};

private:
	// 플레이와 Overlap시 데미지를 주는 간격
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGC_Enemy Damage", meta = (AllowPrivateAccess = true))
	float damageInterval{ 0.1f };

	// 오버랩된 플레이어의 집합
	UPROPERTY()
	TSet<AIGC_Player*> overlappedPlayer{};

	// 데미지를 주는 타이머 핸들
	FTSTicker::FDelegateHandle damageHandle{};

public:
	AIGC_Enemy(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void EndPlay(EEndPlayReason::Type _Reason)override;
	// 적의 초기화를 진행함
	// 비활성화 상태로 만듬
	void InitEnemy();
	// 적의 활성화 상태를 결정함
	// @param _bIsActive true면 적을 활성화 함
	void SetActive(bool _bIsActive);
	// overlap된 플레이어들에게 주기마다 데미지를 부여함
	bool ApplyDamage(float _DeltaTime);

private:
	void OnChangeState(E_CHARACTER_STATE _NewState);
	void OnDelay_ChangeDisable();
	void MoveToNearestPlayer();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex);
};
