#include "InGame/Skill/IGSB_LineTrace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "T_GameInstance.h"

void AIGSB_LineTrace::BeginPlay()
{
	Super::BeginPlay();
	this->SetOwner(GetOwner());
	skillName = TEXT("LineTrace");
	level = 1;
	damage = 25.f;
	duration = 0.5f;
	coolDown = 5.f;
	PTT_LOG(Warning, TEXT("skillName : %s\nlevel : %d\nduration : %0.1f\ncoolDown : %0.1f"), *skillName.ToString(), level, duration, coolDown);
}

void AIGSB_LineTrace::InitSkill()
{
	Super::InitSkill();
}

void AIGSB_LineTrace::UseSkill()
{
	TArray<FHitResult> Hits;
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocation = StartLocation + (GetOwner()->GetActorForwardVector() * 1000.f);

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		StartLocation,
		EndLocation,
		50.f,
		{
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1)
		}
		,
		false,
		{
			GetOwner()
		},
		EDrawDebugTrace::ForDuration,
		Hits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.f
	);
		for (auto& hit : Hits)
		{
			auto* actor = Cast<AActor>(hit.GetActor());
			if (actor)
			{
				actor->TakeDamage(damage, DamageEvent, hit.GetActor()->GetInstigatorController(), GetOwner());
				PTT_LOG(Warning, TEXT("Hit Actor : %s"),*actor->GetName());
			}
		}
	
	PTT_LOG(Warning, TEXT("Use LineTrace"));
	
}
