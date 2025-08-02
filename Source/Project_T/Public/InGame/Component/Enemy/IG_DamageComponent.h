#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_DamageComponent.generated.h"

class UIG_StatComponent;

UCLASS(ClassGroup = (Project_T), meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIG_DamageComponent : public UActorComponent
{
	GENERATED_BODY()
public:

protected:

private:
	// 데미지 컴포넌트의 정보
	UPROPERTY()
	FDamageCompParameter damageInfo{};

	// 오버랩된 스탯 컴포넌트의 집합
	UPROPERTY()
	TSet<UIG_StatComponent*> overlapped{};

	// 데미지를 주는 타이머 핸들
	FTSTicker::FDelegateHandle damageHandle{};

	// 데미지 컴포넌트 활성화 정보
	uint8 bIsActive : 1{};

public:
	// 데미지 컴포넌트를 초기화 한다
	// @param[in] _Param 초기화 시 기재할 정보
	void InitDamageComponent(const FDamageCompParameter& _Param);
	// 데미지 컴포넌트의 활성화 유무를 설정한다
	void ActiveDamageComp(bool _bIsActive);
	// 데미지를 가할 컴포넌트를 추가한다
	// @param[in] _NewOverlap 데미지를 줄 컴포넌트
	void AddOverlapped(TObjectPtr<UIG_StatComponent> _NewOverlap);
	// 데미지를 가하던 컴포넌트를 삭제한다
	// @param[in] _RemoveOverlap 데미지를 해제할 컴포넌트
	void RemoveOverlapped(TObjectPtr<UIG_StatComponent> _RemoveOverlap);
	// 데미지 컴포넌트를 초기화 한다
	void ResetDamageComponent();

protected:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type _Reason) override;

private:
	// overlap된 플레이어들에게 주기마다 데미지를 부여함
	bool Ticker_ApplyDamage(float _DeltaTime);

	//데미지 핸들을 초기화 해준다
	void ClearDamageHandle();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex);

	void OnStateChange(E_CHARACTER_STATE _Prev, E_CHARACTER_STATE _Current, TObjectPtr<UIG_StatComponent> _Target);
};
