#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_TraceComponent.generated.h"

UCLASS(ClassGroup = (Project_T), meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIG_TraceComponent : public UActorComponent
{
	GENERATED_BODY()
public:

protected:

private:
	UPROPERTY()
	TSet<TSubclassOf<AActor>> targetClasses{};

	// 현재 쫓고있는 타겟
	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "UIG_TraceComponent", meta = (AllowPrivateAccess = true))
	AActor* target{};

	//이전 방향
	FVector prevDir{};

	// 데미지 컴포넌트 활성화 정보
	uint8 bIsActive : 1{};

public:
	UIG_TraceComponent(const FObjectInitializer& _Initializer);
	void InitTraceComponent(const TSet<TSubclassOf<AActor>>& _TargetClasses);
	void ActiveTraceComp(bool _bIsActive);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;

private:
	void SetTarget();
	void UpdateTransform();
	void OnChangeTargetState(E_CHARACTER_STATE _PrevState, E_CHARACTER_STATE _NewState);
};
