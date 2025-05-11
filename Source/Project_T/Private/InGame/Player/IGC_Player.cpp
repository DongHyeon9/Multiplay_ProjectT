#include "InGame/Player/IGC_Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "InGame/Player/IG_SkillComponent.h"
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
	camera->SetupAttachment(springArmComp);

	moveAction->ValueType = EInputActionValueType::Axis2D;
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

void AIGC_Player::Move(const FInputActionValue& _Value)
{
	if (Controller == nullptr) return;

	FVector2D movementVector = _Value.Get<FVector2D>();
	PRINTSTR(1, 1.0f, FColor::Cyan, TEXT("%s"), *movementVector.ToString());
	AddMovementInput(GetActorForwardVector(), movementVector.Y);
	AddMovementInput(GetActorRightVector(), movementVector.X);
}
