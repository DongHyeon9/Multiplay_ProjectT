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
	duration = 3;
	coolDown = 5.f;
	PTT_LOG(Warning, TEXT("skillName : %s\nlevel : %d\nduration : %0.1f\ncoolDown : %0.1f"), *skillName.ToString(), level, duration, coolDown);
}

void AIGSB_AOE::BeginPlay()
{
	Super::BeginPlay();
	skillMesh->OnComponentBeginOverlap.AddDynamic(this, &AIGSB_AOE::OnStartOverlap);
}
void AIGSB_AOE::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	if (!skillMesh->bHiddenInGame)
	{
		AddActorLocalOffset(FVector(10.f,0.f,0.f));
	}
}

void AIGSB_AOE::InitSkill()
{
	Super::InitSkill();
}

void AIGSB_AOE::UseSkill()
{
	currentDuration = duration;
	SetActorLocationAndRotation(GetOwner()->GetActorLocation(), GetOwner()->GetActorQuat());
	ActivateSkill(skillMesh);

	PTT_LOG(Warning, TEXT("Use A.O.E"));
}

bool AIGSB_AOE::DamageoverTime(float _DeltaTime)
{
	TArray<FHitResult> Hits;
	FDamageEvent DamageE;
	currentDuration -= 1;

	bool Result = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		GetActorLocation(),
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
			Enemy->TakeDamage(damage, DamageE, GetOwner()->GetInstigatorController(), this);
		}
	}
	
	return currentDuration > 0 ? true : false;
}

void AIGSB_AOE::OnStartOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	const auto actor = Cast<AIGC_Enemy>(_OtherActor);
	if (actor)
	{
		DisableSkill(skillMesh);
		if (durationHandle.IsValid()) { return; }
		durationHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIGSB_AOE::DamageoverTime), 1.f);
	}
}
