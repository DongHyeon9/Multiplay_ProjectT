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

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_UpdateName, VisibleAnywhere, BlueprintReadOnly, category = "AIGC_Player", meta = (AllowPrivateAccess = true))
	FString characterName{};

public:
	AIG_CharacterBase(const FObjectInitializer& _Intializer);

public:
	FORCEINLINE TObjectPtr<UIG_StatComponent> GetStatComp()const { return statComp; }
	FORCEINLINE TObjectPtr<UWidgetComponent> GetStatusWidget()const { return statusWidget; }
	virtual void SetCharacterName(const FString& _NewName);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_UpdateName();

};
