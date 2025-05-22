#pragma once

#include "../Project_T.h"
#include "Animation/AnimInstance.h"
#include "IG_AnimInstance.generated.h"

class AIG_CharacterBase;
class UCharacterMovementComponent;

UCLASS()
class PROJECT_T_API UIG_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	// 현재 애님인스턴스의 오너
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<AIG_CharacterBase> character{};

	// 오너의 무브먼트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCharacterMovementComponent> movementComp{};

	// 오너의 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	FVector velocity{};

	// 오너의 속력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	float speed{};

public:
	void NativeInitializeAnimation();
	void NativeUpdateAnimation(float _DeltaSeconds);

protected:

};
