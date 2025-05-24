#include "InGame/Player/IGC_Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InGame/Player/IG_SkillComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Components/WidgetComponent.h"
#include "InGame/Player/IG_PlayerController.h"
#include "T_GameInstance.h"

AIGC_Player::AIGC_Player(const FObjectInitializer& _Intializer):
	Super(_Intializer)
{
	skillComp = CreateDefaultSubobject<UIG_SkillComponent>(TEXT("SkillComp"));
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	springArmComp->TargetArmLength = 1200.0f;
	springArmComp->bEnableCameraLag = true;
	springArmComp->CameraLagSpeed = 3.0f;
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->bInheritRoll = true;
	springArmComp->bInheritPitch = false;
	springArmComp->bInheritYaw = true;
	springArmComp->bDoCollisionTest = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.f, 0.0f);

	camera->SetupAttachment(springArmComp);
}

void AIGC_Player::SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(_PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(_PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AIGC_Player::Move);
	}
}

void AIGC_Player::BeginPlay()
{
	Super::BeginPlay();

	GetStatusWidget()->SetHiddenInGame(true);

	if (APlayerController* playerController = Cast<APlayerController>(Controller))
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
			subsystem->AddMappingContext(characterMappingContext, 0);

#if !WITH_EDITOR
	//서버에 입장완료를 알린다
	if (HasAuthority()) return;
	if (auto pc = GetController<AIG_PlayerController>())
		pc->Server_InitPlayer(GetGameInstance<UT_GameInstance>()->GetUserName());
#endif
}

void AIGC_Player::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//플레이어 입력을 설정해준다
	characterMappingContext = NewObject<UInputMappingContext>(this);
	moveAction = NewObject<UInputAction>(this);
	moveAction->ValueType = EInputActionValueType::Axis2D;
	auto swizzleAxis = NewObject<UInputModifierSwizzleAxis>(this);
	auto negate = NewObject<UInputModifierNegate>(this);

	characterMappingContext->MapKey(moveAction, EKeys::D);
	characterMappingContext->MapKey(moveAction, EKeys::A).Modifiers.Emplace(negate);
	characterMappingContext->MapKey(moveAction, EKeys::W).Modifiers.Emplace(swizzleAxis);
	auto& moveBackward = characterMappingContext->MapKey(moveAction, EKeys::S);
	moveBackward.Modifiers.Emplace(swizzleAxis);
	moveBackward.Modifiers.Emplace(negate);
}

void AIGC_Player::Move(const FInputActionValue& _Value)
{
	if (Controller == nullptr) return;

	FVector2D movementVector = _Value.Get<FVector2D>();
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), movementVector.Y);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), movementVector.X);
}
