#include "InGame/Skill/IGS_Razer.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InGame/Component/IG_StatComponent.h"
#include "Components/SphereComponent.h"

namespace INGAME_SKILL_RAZER
{
	static constexpr const TCHAR* RAZER_FX_PATH{ TEXT("/Game/08_VFX/NS_Ray") };
}

const FName UIGS_Razer::DETECTION_COMP_NAME{ TEXT("DetectionCollisionComp") };

UIGS_Razer::UIGS_Razer(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FX_RAZER{ INGAME_SKILL_RAZER::RAZER_FX_PATH };

	if (FX_RAZER.Succeeded()) razerFX = FX_RAZER.Object;
}

void UIGS_Razer::InitSkill(const FSkillInitializeParameter& _Initializer)
{
	Super::InitSkill(_Initializer);

	// Owner에 감지용 콜리전 등록
	detectionCollisionComp = NewObject<USphereComponent>(skillInfo.owner, USphereComponent::StaticClass(), DETECTION_COMP_NAME);
	if (detectionCollisionComp)
	{
		detectionCollisionComp->SetupAttachment(skillInfo.owner->GetRootComponent());
		detectionCollisionComp->InitSphereRadius(detectionRange);
		detectionCollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
		detectionCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		for (auto& objectTypeQuery : _Initializer.collisionType)
		{
			ECollisionChannel channel = UEngineTypes::ConvertToCollisionChannel(objectTypeQuery);
			detectionCollisionComp->SetCollisionResponseToChannel(channel, ECollisionResponse::ECR_Overlap);
		}
		detectionCollisionComp->SetGenerateOverlapEvents(true);
		detectionCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		detectionCollisionComp->ComponentTags.Emplace(IG_COLLISION_TAG::DETECTION);
		detectionCollisionComp->RegisterComponent();
		skillInfo.owner->AddInstanceComponent(detectionCollisionComp);
#if WITH_EDITOR
		detectionCollisionComp->SetHiddenInGame(false);
#endif
		if (!detectionCollisionComp->OnComponentBeginOverlap.IsAlreadyBound(this, &UIGS_Razer::OnBeginOverlap))
			detectionCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &UIGS_Razer::OnBeginOverlap);
		if (!detectionCollisionComp->OnComponentEndOverlap.IsAlreadyBound(this, &UIGS_Razer::OnEndOverlap))
			detectionCollisionComp->OnComponentEndOverlap.AddDynamic(this, &UIGS_Razer::OnEndOverlap);
	}
}

void UIGS_Razer::Multicast_Razer_FX_Implementation(const FVector_NetQuantizeNormal& _RazerDirection)
{
	const FVector ownerLocation{ skillInfo.owner->GetActorLocation() };

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		skillInfo.owner->GetWorld(),
		razerFX,
		ownerLocation,
		_RazerDirection.Rotation());
}

void UIGS_Razer::Excute()
{
	SERVER_ONLY();
	const FVector direction{ ComputeRazerDirection() };
	Multicast_Razer_FX(direction);

	TArray<FHitResult> outResult{};

	const int32 currentSkillLevel{ GetSkillLevel() };
	const FVector startLocation{ skillInfo.owner->GetActorLocation() };
	const FVector endLocation{ startLocation + direction * distance };
	if (!UKismetSystemLibrary::SphereTraceMultiForObjects(
		skillInfo.owner->GetWorld(),
		startLocation,
		endLocation,
		range,
		skillInfo.collisionType,
		false,
		{ skillInfo.owner },
#if WITH_EDITOR
		EDrawDebugTrace::ForDuration,
#else
		EDrawDebugTrace::None,
#endif
		outResult,
		true)) return;

	for (auto& result : outResult)
	{
		if (!result.Component.IsValid() || !result.Component->ComponentHasTag(IG_COLLISION_TAG::CHARACTER)) continue;
		auto actor{ result.GetActor() };
		if (!actor) continue;
		ApplyDamage(actor);
	}
}

FVector UIGS_Razer::ComputeRazerDirection() const
{
	// 제일 가까운적에게로
	// 없다면 플레이어의 정면으로
	FVector result{};
	if (detectedActors.IsEmpty())
	{
		result = skillInfo.owner->GetActorForwardVector();
	}
	else
	{
		auto detectedActorList{ detectedActors.Array() };
		const FVector ownerLocation{ skillInfo.owner->GetActorLocation() };
		Algo::Sort(detectedActorList,
			[ownerLocation](const AActor* _Lhs, const AActor* _Rhs) {
				const double distanceL{ FVector::Dist(ownerLocation,_Lhs->GetActorLocation()) };
				const double distanceR{ FVector::Dist(ownerLocation,_Rhs->GetActorLocation()) };
				return distanceL < distanceR;
			});
		result = detectedActorList[0]->GetActorLocation() - skillInfo.owner->GetActorLocation();
		result.Z = 0.0f;
		result.Normalize();
	}
	return result;
}

void UIGS_Razer::OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	if (!_OtherComp->ComponentHasTag(IG_COLLISION_TAG::CHARACTER)) return;
	detectedActors.Emplace(_OtherActor);
}

void UIGS_Razer::OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex)
{
	if (detectedActors.Contains(_OtherActor))
		detectedActors.Remove(_OtherActor);
}
