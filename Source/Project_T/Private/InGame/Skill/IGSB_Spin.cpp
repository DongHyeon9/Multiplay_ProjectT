#include "InGame/Skill/IGSB_Spin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AIGSB_Spin::AIGSB_Spin()
{
	SetReplicates(true);
	SetReplicateMovement(true);

	centerMesh = CreateDefaultSubobject<USphereComponent >(TEXT("Center"));

	spinMesh1 = CreateDefaultSubobject<UStaticMeshComponent >(TEXT("SpinMesh1"));
	spinMesh2 = CreateDefaultSubobject<UStaticMeshComponent >(TEXT("SpinMesh2"));
	spinMesh3 = CreateDefaultSubobject<UStaticMeshComponent >(TEXT("SpinMesh3"));
	spinMesh4 = CreateDefaultSubobject<UStaticMeshComponent >(TEXT("SpinMesh4"));
	spinMesh5 = CreateDefaultSubobject<UStaticMeshComponent >(TEXT("SpinMesh5"));

	centerMesh->SetupAttachment(RootComponent);

	spinMesh1->SetupAttachment(centerMesh);
	spinMesh2->SetupAttachment(centerMesh);
	spinMesh3->SetupAttachment(centerMesh);
	spinMesh4->SetupAttachment(centerMesh);
	spinMesh5->SetupAttachment(centerMesh);

	skillName = TEXT("Spin");
	level = 1;
	duration = 2.f;
	coolDown = 8.f;
	PTT_LOG(Warning, TEXT("skillName : %s\nlevel : %d\nduration : %0.1f\ncoolDown : %0.1f"), *skillName.ToString(), level, duration, coolDown);
}

void AIGSB_Spin::BeginPlay()
{
	Super::BeginPlay();

	spinMeshArray.Emplace(spinMesh1);
	spinMeshArray.Emplace(spinMesh2);
	spinMeshArray.Emplace(spinMesh3);
	spinMeshArray.Emplace(spinMesh4);
	spinMeshArray.Emplace(spinMesh5);

	for (auto& sm : spinMeshArray)
	{
		DisableSkill(sm);
	}
}
void AIGSB_Spin::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	AddActorLocalRotation(FRotator(0.f, 1.f, 0.f));
}

void AIGSB_Spin::InitSkill()
{
	Super::InitSkill();
}

void AIGSB_Spin::UseSkill()
{
	float ference = currentSpin * 6.28;

	SetActorLocation(GetOwner()->GetActorLocation());

	for (int i = 0; i < currentSpin; ++i)
	{
		FVector NewLocation{};
		NewLocation.X = (FMath::Cos(i * ference) * radius);
		NewLocation.Y = (FMath::Sin(i * ference) * radius);
		spinMeshArray[i]->SetWorldLocation(NewLocation);
		ActivateSkill(spinMeshArray[i]);
	}
	PTT_LOG(Warning, TEXT("Use Spin"));
}
