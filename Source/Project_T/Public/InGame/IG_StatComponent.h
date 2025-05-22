#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_StatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeStateDelegate, E_CHARACTER_STATE);

UCLASS( ClassGroup=(Project_T), meta=(BlueprintSpawnableComponent) )
class PROJECT_T_API UIG_StatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// 캐릭터 상태 변경 시 호출되는 델리게이트
	FOnChangeStateDelegate onChangeState{};

private:
	// 현재 캐릭터의 상태
	UPROPERTY(Transient, ReplicatedUsing = OnRep_StateChange, VisibleAnywhere, BlueprintReadOnly, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	E_CHARACTER_STATE characterState{};

public:	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 캐릭터의 상태를 변경
	// @param _NewState 변경될 상태
	void SetCharacterState(E_CHARACTER_STATE _NewState);

private:
	UFUNCTION()
	void OnRep_StateChange();
	
};
