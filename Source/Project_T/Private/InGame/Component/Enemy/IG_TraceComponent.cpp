#include "InGame/Component/Enemy/IG_TraceComponent.h"
#include "InGame/Component/IG_StatComponent.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

UIG_TraceComponent::UIG_TraceComponent(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
}

void UIG_TraceComponent::InitTraceComponent(const TSet<TSubclassOf<AActor>>& _TargetClasses)
{
	targetClasses = _TargetClasses;
}

void UIG_TraceComponent::ActiveTraceComp(bool _bIsActive)
{
	bIsActive = _bIsActive;
	SetTarget();
}

void UIG_TraceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIG_TraceComponent, target);
}

void UIG_TraceComponent::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

	UpdateTransform();
}

void UIG_TraceComponent::SetTarget()
{
	SERVER_ONLY_COMP();

	auto ownerActor{ GetOwner() };
	if (!ownerActor) return;

	// 거리가 가까운 순서대로 타겟을 설정한다
	FVector currentLoc{ ownerActor->GetActorLocation() };
	float nearestDistance{ TNumericLimits<float>::Max() };
	AActor* nearestActor{};
	TActorRange<AActor> actors{ GetWorld() };

	for (auto actor : actors)
	{
		bool bIsTargetClass{};
		for (TSubclassOf<AActor> targetClass : targetClasses)
		{
			if (!actor->IsA(targetClass)) continue;
			bIsTargetClass = true;
			break;
		}

		if (bIsTargetClass)
		{
			const float curDistance{ static_cast<float>((actor->GetActorLocation() - currentLoc).SquaredLength()) };
			if (curDistance >= nearestDistance)continue;

			nearestDistance = curDistance;
			nearestActor = actor;
		}
	}

	target = nearestActor;
	if (!target) return;
	auto statComp{ target->FindComponentByClass<UIG_StatComponent>() };
	if (!statComp) return;
	if (!statComp->onStateChange.IsBoundToObject(this))
		statComp->onStateChange.AddUObject(this, &UIG_TraceComponent::OnChangeTargetState);
}

void UIG_TraceComponent::UpdateTransform()
{
	if (!bIsActive) return;
	if (!target) return;

	auto ownerPawn{ GetOwner<APawn>() };
	if (!ownerPawn) return;

	FVector targetPos{ target->GetActorLocation() };
	FVector curPos{ ownerPawn->GetActorLocation() };
	FVector dir{ targetPos - curPos };
	dir.Normalize();

	ownerPawn->AddMovementInput(dir);

	if (prevDir.Equals(dir)) return;

	prevDir = dir;
	ownerPawn->SetActorRotation(prevDir.Rotation());
}

void UIG_TraceComponent::OnChangeTargetState(E_CHARACTER_STATE _PrevState, E_CHARACTER_STATE _NewState)
{
	if (_NewState == E_CHARACTER_STATE::DEAD)
		SetTarget();
}
