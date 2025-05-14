#pragma once

#include "../Project_T.h"
#include "Animation/AnimInstance.h"
#include "IG_AnimInstance.generated.h"

class AIG_CharacterBase;
class UIG_CharacterMovement;

UCLASS()
class PROJECT_T_API UIG_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<AIG_CharacterBase> character{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_CharacterMovement> movementComp{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	FVector velocity{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	float speed{};

public:
	void NativeInitializeAnimation();
	void NativeUpdateAnimation(float _DeltaSeconds);

protected:

};
