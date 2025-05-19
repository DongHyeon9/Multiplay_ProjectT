#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_StatComponent.generated.h"

UENUM(BlueprintType)
enum class E_CHARACTER_STATE : uint8 {
	DISABLE		UMETA(DisplayName = "Disable"),
	ENABLE		UMETA(DisplayName = "Enable"),
	DEAD		UMETA(DisplayName = "Dead"),
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeStateDelegate, E_CHARACTER_STATE);

UCLASS( ClassGroup=(Project_T), meta=(BlueprintSpawnableComponent) )
class PROJECT_T_API UIG_StatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	FOnChangeStateDelegate onChangeState{};

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_StateChange, VisibleAnywhere, BlueprintReadOnly, category = "UIG_StatComponent", meta = (AllowPrivateAccess = true))
	E_CHARACTER_STATE characterState{};

public:	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_StateChange();
	
};
