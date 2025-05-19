#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "L_PlayerController.generated.h"

class ULW_MatchWait;

UCLASS()
class PROJECT_T_API AL_PlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_PlayerController", meta = (AllowPrivateAccess = true))
	TSubclassOf<ULW_MatchWait> matchWaitClass{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AL_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<ULW_MatchWait> matchWaitWidget{};

public:
	void BeginPlay()override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerName(const FString& _UserName);
	void Server_SetPlayerName_Implementation(const FString& _UserName);
	bool Server_SetPlayerName_Validate(const FString& _UserName);
};
