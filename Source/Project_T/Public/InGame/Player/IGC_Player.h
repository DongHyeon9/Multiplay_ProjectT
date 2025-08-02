#pragma once

#include "InGame/Interface/IG_Character.h"
#include "GameFramework/Character.h"
#include "IGC_Player.generated.h"

class UIG_SkillComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputComponent;
class UIG_PlayerStatComponent;

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class PROJECT_T_API AIGC_Player 
	: public ACharacter
	, public IIG_Character
{
	GENERATED_BODY()
public:
	static const FName CAMERA_NAME;
	static const FName SPRINGARM_NAME;
	static const FName SKILL_COMP_NAME;

private:
	// 플레이어의 스킬이 담길 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_SkillComponent> skillComp{};

	// 카메라의 위치를 정해줄 스프링암
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<USpringArmComponent> springArmComp{};

	// 렌더링할 메인 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> cameraComp{};

	// 캐릭터의 스탯을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_PlayerStatComponent> statComp{};

	// 현재 플레이어의 상태(이름, 체력 등)을 표시하는 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> statusWidget{};

private:
	// 플레이어의 입력을 처리할 맵핑 컨텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Input", meta = (AllowPrivateAccess = true))
	UInputMappingContext* characterMappingContext{};

	// 이동관련 액션
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player Input", meta = (AllowPrivateAccess = true))
	UInputAction* moveAction{};

	//캐릭터의 정보를 담는 구조체
	UPROPERTY(Transient, ReplicatedUsing = OnRep_UpdatePlayerData, VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase", meta = (AllowPrivateAccess = true))
	FPlayerData playerData{};

	TStaticArray<TFunction<void()>, ENetRole::ROLE_MAX> initTable{};

	bool bIsSkillSelectProcessing{};

public:	
	AIGC_Player(const FObjectInitializer& _Intializer);
	void SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent) override;
	virtual void InitPlayerData();
	// 캐릭터의 정보를 설정한다
	// @param _NewData 설정할 데이터
	virtual void SetCharacterData(const FPlayerData& _NewData);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SkillAcquisition(FName _Skill);
	void GameOver();
	void EndGame();

public:
	// 플레이어의 스킬 컴포넌트를 가져온다
	FORCEINLINE TObjectPtr<UIG_SkillComponent> GetSkillComp()const { return skillComp; }
	// 플레이어의 스프링암 컴포넌트를 가져온다
	FORCEINLINE TObjectPtr<USpringArmComponent> GetSpringArmComp()const { return springArmComp; }
	// 플레이어의 카메라를 가져온다
	FORCEINLINE TObjectPtr<UCameraComponent> GetCameraComp()const { return cameraComp; }
	// 플레이어의 정보를 가져온다
	FORCEINLINE const FPlayerData& GetPlayerData()const { return playerData; }
	// 플레이어의 스탯을 가져온다
	TObjectPtr<UIG_StatComponent> GetStatComp()const override;
	// 플레이어의 정보를 표시하는 위젯을 가져온다
	FORCEINLINE TObjectPtr<UWidgetComponent> GetStatusWidget()const override { return statusWidget; }

protected:
	void BeginPlay() override;
	void PreInitializeComponents()override;

private:
	void Move(const FInputActionValue& _Value);

	UFUNCTION()
	void OnRep_UpdatePlayerData();

	UFUNCTION()
	void OnLevelUp(int32 _Prev, int32 _Current);

	void InitPlayerData_SimulatedProxy();
	void InitPlayerData_AutonomousProxy();
	void InitPlayerData_Authority();

	void SkillAcquisitionProcess();

	void OnChangeState(E_CHARACTER_STATE _Prev, E_CHARACTER_STATE _Current);
};
