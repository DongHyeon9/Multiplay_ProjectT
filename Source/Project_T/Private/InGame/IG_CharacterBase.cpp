#include "InGame/IG_CharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/IG_StatComponent.h"
#include "Components/WidgetComponent.h"
#include "InGame/Widget/IGW_CharacterStatus.h"
#include "Net/UnrealNetwork.h"

AIG_CharacterBase::AIG_CharacterBase(const FObjectInitializer& _Intializer)
	: Super(_Intializer)
{
	PrimaryActorTick.bCanEverTick = true;

	statComp = CreateDefaultSubobject<UIG_StatComponent>(TEXT("StatComp"));
	statusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusWidget"));
	statusWidget->SetupAttachment(RootComponent);
	statusWidget->bHiddenInGame = true;
	statusWidget->SetDrawSize(FVector2D{ 250.0f, 250.0f });
	statusWidget->SetWidgetSpace(EWidgetSpace::Screen);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FClassFinder<UUserWidget> WG_STATUS(TEXT("/Game/01_Blueprint/InGame/Widget/WG_IGW_CharacterStatus"));
	if (WG_STATUS.Succeeded()) statusWidget->SetWidgetClass(WG_STATUS.Class);
}

void AIG_CharacterBase::SetCharacterName(const FString& _NewName)
{
	characterName = _NewName;
}

void AIG_CharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_CharacterBase, characterName);
}

void AIG_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	RootComponent->SetHiddenInGame(false);
#endif
}

void AIG_CharacterBase::OnRep_UpdateName()
{
	SetCharacterName(characterName);

	if (characterName.StartsWith(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive))
		characterName = characterName.RightChop(PLAYER_NAME_PREFIX.Len());

	if (auto widget = Cast<UIGW_CharacterStatus>(statusWidget->GetWidget()))
		widget->SetName(FText::FromString(characterName));
}
