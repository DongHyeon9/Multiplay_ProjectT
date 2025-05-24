#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_StatComponent.generated.h"

USTRUCT(BlueprintType)
struct F_CHARACTER_STATUS
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 level{ 1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float damage{ 10.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float attackRange{ 200.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxEXP{ 100.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float currentEXP{ 0.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float nextLevelEXP{ 100.f };
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeStateDelegate, E_CHARACTER_STATE);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeStatusDelegate, F_CHARACTER_STATUS);

UCLASS( ClassGroup=(Project_T), meta=(BlueprintSpawnableComponent) )
class PROJECT_T_API UIG_StatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UIG_StatComponent();
	
	// 캐릭터 상태 변경 시 호출되는 델리게이트
	FOnChangeStateDelegate onChangeState{};

	// 캐릭터 스테이터스 변경시 호출되는 델리게이트
	FOnChangeStatusDelegate onChangeStatus{};
private:
	// 현재 캐릭터의 상태
	UPROPERTY(Transient, ReplicatedUsing = OnRep_StateChange, VisibleAnywhere, BlueprintReadOnly, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	E_CHARACTER_STATE characterState{};

	// 현재 캐릭터의 스테이터스
	UPROPERTY(Transient, ReplicatedUsing = OnRep_StatusChange, VisibleAnywhere, BlueprintReadOnly, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	F_CHARACTER_STATUS characterStatus{};

public:	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 캐릭터의 상태를 변경
	// @param _NewState 변경될 상태
	void SetCharacterState(E_CHARACTER_STATE _NewState);

	// 캐릭터 스테이터스 변경
	void SetCharacterStatus(F_CHARACTER_STATUS _NewStatus);

	// 경험치 얻는 로직 (서버)
	// @param _Exp는 경험치량이다
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void Server_AddExperience(int32 _Exp);

	// 경험치 얻는 로직 
	// @param _Exp는 경험치량이다
	UFUNCTION(BlueprintCallable)
	void AddExperience(int32 _Exp);

	UFUNCTION(BlueprintCallable)
	void LevelUp();

	FORCEINLINE const F_CHARACTER_STATUS& GetStats() const { return characterStatus; }

private:
	UFUNCTION()
	void OnRep_StateChange();

	UFUNCTION()
	void OnRep_StatusChange();
};
