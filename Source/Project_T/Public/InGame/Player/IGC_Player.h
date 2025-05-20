#pragma once

#include "../Project_T.h"
#include "InGame/IG_CharacterBase.h"
#include "IGC_Player.generated.h"

class UIG_SkillComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputComponent;

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class PROJECT_T_API AIGC_Player : public AIG_CharacterBase
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_SkillComponent> skillComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<USpringArmComponent> springArmComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> camera{};

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Input", meta = (AllowPrivateAccess = true))
	UInputMappingContext* characterMappingContext{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Input", meta = (AllowPrivateAccess = true))
	UInputAction* moveAction{};

public:	
	AIGC_Player(const FObjectInitializer& _Intializer);
	void SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent) override;

public:
	FORCEINLINE TObjectPtr<UIG_SkillComponent> GetSkillComp()const { return skillComp; }
	FORCEINLINE TObjectPtr<USpringArmComponent> GetSpringArmComp()const { return springArmComp; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetCameraComp()const { return camera; }

protected:
	void BeginPlay() override;
	void PreInitializeComponents()override;

private:
	void Move(const FInputActionValue& _Value);

};
