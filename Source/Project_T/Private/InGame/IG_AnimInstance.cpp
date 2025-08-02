#include "InGame/IG_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UIG_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	auto pawn = TryGetPawnOwner();

	if (!pawn) return;
	
	movementComp = pawn->GetMovementComponent();
}

void UIG_AnimInstance::NativeUpdateAnimation(float _DeltaSeconds)
{
	Super::NativeUpdateAnimation(_DeltaSeconds);

	if (!movementComp) return;

	velocity = movementComp->Velocity;
	speed = velocity.Length();
}
