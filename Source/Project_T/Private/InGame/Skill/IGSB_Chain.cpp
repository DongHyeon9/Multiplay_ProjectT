#include "InGame/Skill/IGSB_Chain.h"
#include "InGame/Enemy/IGC_Enemy.h"
#include "Kismet/GameplayStatics.h"

void AIGSB_Chain::BeginPlay()
{
	this->SetOwner(GetOwner());
	skillName = TEXT("Chain");
	level = 1;
	damage = 12.f;
	duration = 0.5f;
	coolDown = 5.f;
	PTT_LOG(Warning, TEXT("skillName : %s\nlevel : %d\nduration : %0.1f\ncoolDown : %0.1f"), *skillName.ToString(), level, duration, coolDown);
}

void AIGSB_Chain::InitSkill()
{
	Super::InitSkill();
}

void AIGSB_Chain::UseSkill()
{
	TArray<AActor*> Targets;
	TArray<AIGC_Enemy*> Enemys;

	auto* CurrentTarget = UGameplayStatics::GetActorOfClass(GetWorld(), AActor::StaticClass());

	for (int i = 0; i < maxChain && IsValid(CurrentTarget); ++i)
	{
		
		//UGameplayStatics::ApplyDamage(CurrentTarget, damage, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
		CurrentTarget->TakeDamage(damage, DamageEvent, GetOwner()->GetInstigatorController(), GetOwner());
		PTT_LOG(Warning, TEXT("Target : %s"), *CurrentTarget->GetName());
		Targets.Add(CurrentTarget);

		TArray<AActor*> Candidates;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Candidates);

		Candidates.RemoveAll([&](AActor* Actor) {
			return Targets.Contains(Actor);
			});

		auto* NextTarget = (UGameplayStatics::FindNearestActor(CurrentTarget->GetActorLocation(), Targets, chainDistance));
		if (!NextTarget) break;


		if (IsValid(NextTarget))
		{
			//Effect
		}

		CurrentTarget = NextTarget;
	}
	PTT_LOG(Warning, TEXT("Use Chain"));
}
