#include "InGame/Skill/IGS_Slash.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "InGame/Component/IG_StatComponent.h"

namespace INGAME_SKILL_SLASH
{
	static constexpr const TCHAR* SLASH_FX_PATH{ TEXT("/Game/08_VFX/NS_Slash") };
	static constexpr const TCHAR* HIT_FX_PATH{ TEXT("/Game/08_VFX/NS_Slash_Impact") };
}

UIGS_Slash::UIGS_Slash(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FX_SLASH{ INGAME_SKILL_SLASH::SLASH_FX_PATH };
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FX_HIT{ INGAME_SKILL_SLASH::HIT_FX_PATH };

	if (FX_SLASH.Succeeded()) slashFX = FX_SLASH.Object;
	if (FX_HIT.Succeeded()) hitFX = FX_HIT.Object;
}

void UIGS_Slash::Multicast_Excute_Implementation()
{
	Super::Multicast_Excute_Implementation();

	const int32 currentSkillLevel{ GetSkillLevel() };
	const FVector ownerLocation{ skillInfo.owner->GetActorLocation() };

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		skillInfo.owner->GetWorld(),
		slashFX,
		ownerLocation,
		UKismetMathLibrary::ComposeRotators(rotationOffset, skillInfo.owner->GetActorRotation()),
		baseSacle + scaleOfLevel * currentSkillLevel);
	
	TArray<FHitResult> outResult{};
	if (!DetectTarget(outResult)) return;

	for (const auto& result : outResult)
	{
		if (!result.Component.IsValid() || !result.Component->ComponentHasTag(IG_COLLISION_TAG::CHARACTER)) continue;
		auto actor{ result.GetActor() };
		if (!actor) continue;
		auto statComp{ actor->FindComponentByClass<UIG_StatComponent>() };
		if (!statComp) continue;
		// 오너보다 뒤에 있으면 슬래쉬는 맞지 않는다
		FVector ownerToTargetDir{ (actor->GetActorLocation() - ownerLocation) };
		ownerToTargetDir.Normalize();
		if (FVector::DotProduct(ownerToTargetDir, skillInfo.owner->GetActorForwardVector()) < 0.0) continue;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			skillInfo.owner->GetWorld(),
			hitFX,
			actor->GetActorLocation(),
			actor->GetActorRotation(),
			baseSacle + scaleOfLevel * currentSkillLevel);
	}
}

void UIGS_Slash::Excute()
{
	SERVER_ONLY();
	
	Super::Excute();

	TArray<FHitResult> outResult{};
	if (!DetectTarget(outResult)) return;

	const int32 currentSkillLevel{ GetSkillLevel() };
	const FVector ownerLocation{ skillInfo.owner->GetActorLocation() };

	for (auto& result : outResult)
	{
		auto actor{ result.GetActor() };
		if (!actor) continue;
		// 오너보다 뒤에 있으면 슬래쉬는 맞지 않는다
		FVector ownerToTargetDir{ (actor->GetActorLocation() - ownerLocation) };
		ownerToTargetDir.Normalize();
		if (FVector::DotProduct(ownerToTargetDir, skillInfo.owner->GetActorForwardVector()) < 0.0) continue;

		ApplyDamage(actor);
	}
}

bool UIGS_Slash::DetectTarget(TArray<FHitResult>& _OutResult)
{
	const int32 currentSkillLevel{ GetSkillLevel() };
	const FVector ownerLocation{ skillInfo.owner->GetActorLocation() };

	return 	UKismetSystemLibrary::SphereTraceMultiForObjects(
		skillInfo.owner->GetWorld(),
		ownerLocation,
		ownerLocation,
		baseRange + rangeOfLevel * currentSkillLevel,
		skillInfo.collisionType,
		false,
		{ skillInfo.owner },
#if WITH_EDITOR
		EDrawDebugTrace::ForDuration,
#else
		EDrawDebugTrace::None,
#endif
		_OutResult,
		true);
}
