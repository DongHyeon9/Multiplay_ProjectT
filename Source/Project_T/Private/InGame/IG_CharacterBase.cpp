#include "InGame/IG_CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/IG_StatComponent.h"
#include "Components/WidgetComponent.h"
#include "InGame/Widget/IGW_CharacterStatus.h"
#include "Net/UnrealNetwork.h"

#define CREATE_DYNAMIC_MATERIAL(idx) GetMesh()->SetMaterial(idx, UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(idx), this))
#define SET_MESH_COLOR(idx) 	auto mat = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(idx));					\
								if (mat) mat->SetVectorParameterValue(TEXT("Tint"), characterData.characterColor)		\

AIG_CharacterBase::AIG_CharacterBase(const FObjectInitializer& _Intializer)
	: Super(_Intializer)
{
	PrimaryActorTick.bCanEverTick = true;

	statComp = CreateDefaultSubobject<UIG_StatComponent>(TEXT("StatComp"));
	statusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusWidget"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	statusWidget->SetupAttachment(RootComponent);
	statusWidget->bHiddenInGame = true;
	statusWidget->SetDrawSize(FVector2D{ 250.0f, 250.0f });
	statusWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ENEMY(TEXT("/Game/02_Mesh/SKM_Manny"));
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_ENEMY(TEXT("/Game/06_Animation/ABP_Manny"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WG_STATUS(TEXT("/Game/01_Blueprint/InGame/Widget/WG_IGW_CharacterStatus"));

	if (SK_ENEMY.Succeeded()) GetMesh()->SetSkeletalMesh(SK_ENEMY.Object);
	if (ABP_ENEMY.Succeeded()) GetMesh()->SetAnimInstanceClass(ABP_ENEMY.Class);
	if (WG_STATUS.Succeeded()) statusWidget->SetWidgetClass(WG_STATUS.Class);
}

void AIG_CharacterBase::SetCharacterData(const FCharacterData& _NewData)
{
	PTT_LOG(Warning, TEXT("%s"), *_NewData.ToString());
	characterData = _NewData;
}

void AIG_CharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_CharacterBase, characterData);
}

void AIG_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		// 다이나믹 머티리얼을 생성한다
		int32 materialCount{ GetMesh()->GetMaterials().Num() };
		for (int32 i = 0; i < materialCount; ++i)
		{
			CREATE_DYNAMIC_MATERIAL(i);
		}
	}

#if WITH_EDITOR
	RootComponent->SetHiddenInGame(false);
#endif
}

void AIG_CharacterBase::OnRep_UpdateCharacterData()
{
	//서버에서 캐릭터의 정보가 변경되면 클라이언트에서 업데이트한다
	if (characterData.characterName.StartsWith(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive))
		characterData.characterName = characterData.characterName.RightChop(PLAYER_NAME_PREFIX.Len());

	if (auto widget = Cast<UIGW_CharacterStatus>(statusWidget->GetWidget()))
		widget->SetName(FText::FromString(characterData.characterName));

	int32 materialCount{ GetMesh()->GetMaterials().Num() };
	for (int32 i = 0; i < materialCount; ++i)
	{
		SET_MESH_COLOR(i);
	}
}

#undef CREATE_DYNAMIC_MATERIAL
#undef SET_MESH_COLOR