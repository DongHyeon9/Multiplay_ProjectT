#include "InGame/Skill/IGSB_AOE.h"
#include "InGame/Enemy/IGC_Enemy.h"

#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AIGSB_AOE::AIGSB_AOE()
{
	SetReplicates(true);
	SetReplicateMovement(true);

	skillMesh = CreateDefaultSubobject<UStaticMeshComponent >(TEXT("SkillMesh"));

	DisableSkill(skillMesh);

	this->SetOwner(GetOwner());
	skillName = TEXT("AOE");
	level = 1;
	damage = 10.f;
	duration = 3.f;
	coolDown = 5.f;
	PTT_LOG(Warning, TEXT("skillName : %s\nlevel : %d\nduration : %0.1f\ncoolDown : %0.1f"), *skillName.ToString(), level, duration, coolDown);
}

void AIGSB_AOE::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIGSB_AOE::InitSkill()
{
	Super::InitSkill();
}

void AIGSB_AOE::UseSkill()
{
	SetActorLocation(GetOwner()->GetActorLocation());

	PTT_LOG(Warning, TEXT("Use A.O.E"));
}

void AIGSB_AOE::DamageoverTime()
{
	TArray<FHitResult> Hits;
	//FDamageEvent DamageEvent;

	bool Result = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		GetActorLocation() * GetOwner()->GetActorForwardVector(),
		GetActorLocation(),
		radius,
		{
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1),
		},
		false,
		{
			GetOwner(),this,
		},
		EDrawDebugTrace::ForOneFrame,
		Hits,
		true
	);

	if (Result)
	{
		for (auto& hit : Hits)
		{
			auto Enemy = Cast<AIGC_Enemy>(hit.GetActor());
			// Damage
		}
	}

}
