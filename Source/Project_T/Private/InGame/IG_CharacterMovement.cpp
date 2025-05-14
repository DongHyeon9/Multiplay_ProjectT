#include "InGame/IG_CharacterMovement.h"
#include "InGame/IG_CharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

UIG_CharacterMovement::UIG_CharacterMovement()
{
	SetIsReplicatedByDefault(true);
}

void UIG_CharacterMovement::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		velocityRep = Velocity;
		Server_Velocity(velocityRep);
	}

	if (!velocityRep.IsNearlyZero())
	{
		float moveSpeed{};
		velocityRep.ToDirectionAndLength(moveDirection, moveSpeed);

		FRotator targetRot = UKismetMathLibrary::ComposeRotators(moveDirection.Rotation(), rotationOffset);

		targetRot.Pitch = 0.0f;
		targetRot.Roll = 0.0f;

		FRotator currentRot = meshComp->GetComponentRotation();
		FRotator newRot{ FMath::RInterpTo(currentRot, targetRot, _DeltaTime, rotationSpeed) };
		meshComp->SetWorldRotation(newRot);
	}
}

void UIG_CharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	owner = Cast<AIG_CharacterBase>(GetOwner());
	if (!owner) return;
	meshComp = owner->GetSkeletalMeshComp();
	if (!meshComp) return;

	rotationOffset = meshComp->GetComponentRotation();
}

void UIG_CharacterMovement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIG_CharacterMovement, velocityRep);
	DOREPLIFETIME(UIG_CharacterMovement, moveDirection);
}

void UIG_CharacterMovement::Server_Velocity_Implementation(const FVector_NetQuantize100& _Velocity)
{
	velocityRep = _Velocity;
	Velocity = _Velocity;
}
