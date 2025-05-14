#pragma once

#include "../Project_T.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "IG_CharacterMovement.generated.h"

class AIG_CharacterBase;

UCLASS()
class PROJECT_T_API UIG_CharacterMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()
	
private:
	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "UIG_CharacterMovement", meta = (AllowPrivateAccess = true))
	FVector velocityRep{};

	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "UIG_CharacterMovement", meta = (AllowPrivateAccess = true))
	FVector moveDirection{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "UIG_CharacterMovement", meta = (AllowPrivateAccess = true))
	float rotationSpeed{ 10.0f };

	UPROPERTY()
	AIG_CharacterBase* owner{};

	UPROPERTY()
	USkeletalMeshComponent* meshComp{};

	UPROPERTY()
	FRotator rotationOffset{};

public:
	UIG_CharacterMovement();
	void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)override;
	void BeginPlay()override;

public:
	FORCEINLINE FVector GetVelocity()const { return velocityRep; }

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const  override;

private:
	UFUNCTION(Server, UnReliable, WithValidation)
	void Server_Velocity(const FVector_NetQuantize100& _Velocity);
	void Server_Velocity_Implementation(const FVector_NetQuantize100& _Velocity);
	bool Server_Velocity_Validate(const FVector_NetQuantize100& _Velocity) { return true; }
};
