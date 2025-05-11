#pragma once

#include "../Project_T.h"
#include "GameFramework/Pawn.h"
#include "IG_CharacterBase.generated.h"

class UInputComponent;
class UPawnMovementComponent;
class USkeletalMeshComponent;
class USphereComponent;
class UIG_StatComponent;

UCLASS(Abstract)
class PROJECT_T_API AIG_CharacterBase : public APawn
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> rootComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UPawnMovementComponent> movementComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> meshComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_StatComponent> statComp{};

public:
	AIG_CharacterBase(const FObjectInitializer& _Intializer);

public:
	FORCEINLINE TObjectPtr<USphereComponent> GetSphereComp()const { return rootComp; }
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetSkeletalMeshComp()const { return meshComp; }
	FORCEINLINE TObjectPtr<UIG_StatComponent> GetStatComp()const { return statComp; }

protected:
	void BeginPlay() override;

};
