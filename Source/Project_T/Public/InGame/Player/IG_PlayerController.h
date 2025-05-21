#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "IG_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UIGW_Main;

UCLASS()
class PROJECT_T_API AIG_PlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_PlayerController Input", meta = (AllowPrivateAccess = true))
	UInputMappingContext* playerMappingContext{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_PlayerController", meta = (AllowPrivateAccess = true))
	TSubclassOf<UIGW_Main> mainWidgetClass{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIGW_Main> mainWidget{};

public:
	AIG_PlayerController(const FObjectInitializer& _Initializer);
	void PreInitializeComponents()override;
	void InitPlayer(const FCharacterData& _NewData);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InitPlayer(const FString& _NewName);
	void Server_InitPlayer_Implementation(const FString& _NewName);
	bool Server_InitPlayer_Validate(const FString& _NewName);

	UFUNCTION(Server, Reliable)
	void Server_OnFinishStartEvent();
	void Server_OnFinishStartEvent_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_StartGame();
	void Client_StartGame_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_StartEvent();
	void Client_StartEvent_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_EndEvent();
	void Client_EndEvent_Implementation();

protected:
	void BeginPlay()override;
	void SetupInputComponent()override;

private:
	UFUNCTION()
	void PrintResult();
};
