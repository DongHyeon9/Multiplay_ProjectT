#include "InGame/IG_AnimInstance.h"
#include "InGame/IG_CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UIG_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	character = Cast<AIG_CharacterBase>(GetOwningActor());

	if (!character) return;

	movementComp = character->GetCharacterMovement();
}

void UIG_AnimInstance::NativeUpdateAnimation(float _DeltaSeconds)
{
	Super::NativeUpdateAnimation(_DeltaSeconds);

	if (!character) return;

	velocity = movementComp->Velocity;
	speed = velocity.Length();
}
