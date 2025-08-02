#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_StatComponent.generated.h"

UCLASS(Abstract)
class PROJECT_T_API UIG_StatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	FOnStateChangeDelegate onStateChange{};
	FOnHPChangeDelegate onHPChange{};
	FOnDataChangeDelegate onDataChange{};

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_StatComponent")
	UDataTable* statDataTable{};

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_StateChange, BlueprintReadOnly, VisibleAnywhere, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	E_CHARACTER_STATE characterState{};

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HPChange, BlueprintReadOnly, VisibleAnywhere, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	float currentHP{};

	UPROPERTY(Transient, ReplicatedUsing = OnRep_DataChange, BlueprintReadOnly, VisibleAnywhere, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	FName rowName{};

	const FCharacterData* characterData{};

public:
	UIG_StatComponent(const FObjectInitializer& _Initializer);
	FORCEINLINE E_CHARACTER_STATE GetCharacterState()const { return characterState; }
	FORCEINLINE float GetCurrentHP()const { return currentHP; }
	FORCEINLINE float GetDamage()const { return characterData->defaultDamage; }
	template<class _T>
	FORCEINLINE const _T* GetCharacterData()const { return reinterpret_cast<const _T*>(characterData); }
	FORCEINLINE const FCharacterData* GetCharacterData()const { return characterData; }

#pragma region 서버 전용 코드
	virtual void InitStat(int32 _Idx);
	// 캐릭터의 데이터를 설정한다
	// @param[in] _Idx 데이터 테이블의 인덱스(zero base index)
	virtual void SetDataTable(int32 _Idx);
	// 대상의 상태를 설정한다
	// @param[in] _NewState 설정할 상태
	void SetCharacterState(E_CHARACTER_STATE _NewState);
	// 대상의 HP를 설정해준다
	// @param[in] _NewHP 설정할 HP의 양
	void SetHP(float _NewHP);
	// 대상의 HP를 더해준다
	// @param[in] _AddHP 더할 HP의 양
	void AddHP(float _AddHP);
	// 대상에게 데미지를 주고 체력이 0이하가 되면 상태를 DEAD로 변경
	// @param[in] _Instigator 가해자의 컨트롤러
	// @param[in] _Actor 가해자가 사용한 무기
	// @param[in] _Damage 가해지는 데미지량
	virtual void ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage);
#pragma endregion 서버 전용 코드

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

private:
	UFUNCTION()	void OnRep_StateChange(E_CHARACTER_STATE _Prev);
	UFUNCTION()	void OnRep_HPChange(float _Prev);
	UFUNCTION()	void OnRep_DataChange(FName _Prev);
};