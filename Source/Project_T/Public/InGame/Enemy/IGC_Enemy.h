#pragma once

#include "InGame/Interface/IG_Character.h"
#include "GameFramework/Pawn.h"
#include "Containers/Ticker.h"
#include "IGC_Enemy.generated.h"

class AIGC_Player;
class UFloatingPawnMovement;
class UCapsuleComponent;
class AIGC_Enemy;
class UIG_EnemyStatComponent;
class UIG_DamageComponent;
class UIG_TraceComponent;

enum class E_CHARACTER_STATE : uint8;

UCLASS()
class PROJECT_T_API AIGC_Enemy 
	: public APawn
	, public IIG_Character
{
	GENERATED_BODY()
public:
	// 상태가 변경될 때 호출되는 델리게이트
	FOnEnemyStateDelegate onEnemyState{};

	static const FName CAPSULE_NAME;
	static const FName MESH_NAME;
	static const FName MOVEMENT_NAME;
	static const FName DAMAGE_NAME;
	static const FName TRACE_NAME;

private:
	// 적의 기본이 되는 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Enemy Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> rootComp{};

	// 캐릭터의 스탯을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Enemy Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_EnemyStatComponent> statComp{};

	// 현재 플레이어의 상태(이름, 체력 등)을 표시하는 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Enemy Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> statusWidget{};

	// 적의 메시를 나타낼 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Enemy Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> meshComp{};

	// 적의 이동을 담당하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Enemy Default", meta = (AllowPrivateAccess = true))
	UFloatingPawnMovement* enemyMovementComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Enemy Default", meta = (AllowPrivateAccess = true))
	UIG_DamageComponent* damageComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Enemy Default", meta = (AllowPrivateAccess = true))
	UIG_TraceComponent* traceComp{};

private:
	//몬스터가 죽었을때 비활성화되는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGC_Enemy", meta = (AllowPrivateAccess = true))
	float disableDelay{ 3.0f };

	//비활성화 타이머 핸들
	FTimerHandle disableHandle{};

	TStaticArray<TFunction<void()>, static_cast<int32>(E_CHARACTER_STATE::END)> changeStateTable{};

public:
	AIGC_Enemy(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void ActiveEnemy();

public:
	//적의 움직임을 담당하는 컴포넌트를 가져온다
	UPawnMovementComponent* GetMovementComponent() const override;
	//적의 스탯을 담당하는 컴포넌트를 가져온다
	TObjectPtr<UIG_StatComponent> GetStatComp()const override;
	//적의 정보를 표시하는 위젯을 가져온다
	FORCEINLINE TObjectPtr<UWidgetComponent> GetStatusWidget()const override { return statusWidget; }
	//적의 메인 콜리전을 가져온다
	FORCEINLINE TObjectPtr<UCapsuleComponent> GetRootComp() const { return rootComp; }
	//적의 메시를 가져온다
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetMeshComp() const { return meshComp; }

private:
	void OnChangeState(E_CHARACTER_STATE _PrevState, E_CHARACTER_STATE _NewState);
	void OnChangeHP(float _PrevHP, float _CurrentHP);

	void OnChangeState_Server_Enable();
	void OnChangeState_Server_Disable();
	void OnChangeState_Server_Dead();

	void OnChangeState_Client_Enable();
	void OnChangeState_Client_Disable();
	void OnChangeState_Client_Dead();

	void SetRagdoll(bool _ActiveRagdoll);

};
