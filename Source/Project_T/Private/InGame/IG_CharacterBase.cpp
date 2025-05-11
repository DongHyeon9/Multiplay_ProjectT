#include "InGame/IG_CharacterBase.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/IG_StatComponent.h"

AIG_CharacterBase::AIG_CharacterBase(const FObjectInitializer& _Intializer):
	Super(_Intializer)
{
	PrimaryActorTick.bCanEverTick = true;

	movementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	rootComp = CreateDefaultSubobject<USphereComponent>(TEXT("RootComp"));
	statComp = CreateDefaultSubobject<UIG_StatComponent>(TEXT("StatComp"));

	RootComponent = rootComp;
	rootComp->InitSphereRadius(50.0f);

	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

void AIG_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	rootComp->SetHiddenInGame(false);
#endif

}