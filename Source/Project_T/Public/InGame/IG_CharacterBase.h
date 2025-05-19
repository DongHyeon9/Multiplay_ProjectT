#pragma once

#include "../Project_T.h"
#include "GameFramework/Character.h"
#include "IG_CharacterBase.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USphereComponent;
class UIG_StatComponent;
class UWidgetComponent;

UCLASS(Abstract)
class PROJECT_T_API AIG_CharacterBase : public ACharacter
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_StatComponent> statComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> statusWidget{};

public:
	AIG_CharacterBase(const FObjectInitializer& _Intializer);

public:
	FORCEINLINE TObjectPtr<UIG_StatComponent> GetStatComp()const { return statComp; }
	FORCEINLINE TObjectPtr<UWidgetComponent> GetStatusWidget()const { return statusWidget; }

protected:
	void BeginPlay() override;

private:

};
