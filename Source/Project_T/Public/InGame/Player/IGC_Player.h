#pragma once

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
	// 플레이어의 스킬이 담길 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_SkillComponent> skillComp{};

	// 카메라의 위치를 정해줄 스프링암
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<USpringArmComponent> springArmComp{};

	// 렌더링할 메인 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> camera{};

private:
	// 플레이어의 입력을 처리할 맵핑 컨텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Input", meta = (AllowPrivateAccess = true))
	UInputMappingContext* characterMappingContext{};

	// 이동관련 액션
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Input", meta = (AllowPrivateAccess = true))
	UInputAction* moveAction{};

public:	
	AIGC_Player(const FObjectInitializer& _Intializer);
	void SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent) override;

public:
	// 플레이어의 스킬 컴포넌트를 가져온다
	FORCEINLINE TObjectPtr<UIG_SkillComponent> GetSkillComp()const { return skillComp; }
	// 플레이어의 스프링암 컴포넌트를 가져온다
	FORCEINLINE TObjectPtr<USpringArmComponent> GetSpringArmComp()const { return springArmComp; }
	// 플레이어의 카메라를 가져온다
	FORCEINLINE TObjectPtr<UCameraComponent> GetCameraComp()const { return camera; }

protected:
	void BeginPlay() override;
	void PreInitializeComponents()override;

private:
	void Move(const FInputActionValue& _Value);

};
