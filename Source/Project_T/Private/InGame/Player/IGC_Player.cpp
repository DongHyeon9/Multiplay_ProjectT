#include "InGame/Player/IGC_Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InGame/Player/IG_SkillComponent.h"
#include "InGame/IG_CharacterMovement.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

AIGC_Player::AIGC_Player(const FObjectInitializer& _Intializer):
	Super(_Intializer)
{
	skillComp = CreateDefaultSubobject<UIG_SkillComponent>(TEXT("SkillComp"));
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	characterMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("CharacterMappingContext"));
	moveAction = CreateDefaultSubobject<UInputAction>(TEXT("MoveAction"));

	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	springArmComp->TargetArmLength = 1200.0f;
	springArmComp->bEnableCameraLag = true;
	springArmComp->CameraLagSpeed = 3.0f;

	GetCharacterMovementComp()->MaxSpeed = 600.0f;

	camera->SetupAttachment(springArmComp);

	moveAction->ValueType = EInputActionValueType::Axis2D;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PLAYER(TEXT("/Game/02_Mesh/PlayerCharacter/SKM_Manny"));
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_PLAYER(TEXT("/Game/06_Animation/PlayerCharacter/ABP_Manny"));

	if (SK_PLAYER.Succeeded()) GetSkeletalMeshComp()->SetSkeletalMesh(SK_PLAYER.Object);
	if (ABP_PLAYER.Succeeded())	GetSkeletalMeshComp()->SetAnimInstanceClass(ABP_PLAYER.Class);
}

void AIGC_Player::SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(_PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(_PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Started, this, &AIGC_Player::MoveStart);
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AIGC_Player::Move);
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Completed, this, &AIGC_Player::MoveEnd);
	}
}

void AIGC_Player::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (bIsMove && GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		Server_Move(GetActorLocation());
	}
}

void AIGC_Player::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(characterMappingContext, 0);
		}
	}
}

void AIGC_Player::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	auto swizzleAxis = NewObject<UInputModifierSwizzleAxis>();
	auto negate = NewObject<UInputModifierNegate>();

	characterMappingContext->MapKey(moveAction, EKeys::D);
	characterMappingContext->MapKey(moveAction, EKeys::A).Modifiers.Emplace(negate);
	characterMappingContext->MapKey(moveAction, EKeys::W).Modifiers.Emplace(swizzleAxis);
	auto& moveBackward = characterMappingContext->MapKey(moveAction, EKeys::S);
	moveBackward.Modifiers.Emplace(swizzleAxis);
	moveBackward.Modifiers.Emplace(negate);
}

void AIGC_Player::MoveStart()
{
	bIsMove = true;
}

void AIGC_Player::Move(const FInputActionValue& _Value)
{
	if (Controller == nullptr) return;

	FVector2D movementVector = _Value.Get<FVector2D>();
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), movementVector.Y);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), movementVector.X);
}

void AIGC_Player::MoveEnd()
{
	bIsMove = false;
}

void AIGC_Player::Server_Move_Implementation(const FVector_NetQuantize100& _Transform)
{
	SetActorLocation(_Transform);
}
