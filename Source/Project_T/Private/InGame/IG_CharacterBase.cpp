#include "InGame/IG_CharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/IG_StatComponent.h"
#include "Components/WidgetComponent.h"

AIG_CharacterBase::AIG_CharacterBase(const FObjectInitializer& _Intializer)
	: Super(_Intializer)
{
	PrimaryActorTick.bCanEverTick = true;

	statComp = CreateDefaultSubobject<UIG_StatComponent>(TEXT("StatComp"));
	statusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusWidget"));
	statusWidget->SetupAttachment(RootComponent);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
}

void AIG_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	RootComponent->SetHiddenInGame(false);
#endif
}