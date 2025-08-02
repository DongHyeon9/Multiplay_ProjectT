#pragma once

#include "../Project_T.h"
#include "Animation/AnimInstance.h"
#include "IG_AnimInstance.generated.h"

UCLASS()
class PROJECT_T_API UIG_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	// 오너의 무브먼트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_AnimInstance Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UPawnMovementComponent> movementComp{};

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
