#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_ManagerBase.generated.h"

class AIG_PlayerController;
class AIGC_Player;

UCLASS(ClassGroup = (Project_T), Abstract)
class PROJECT_T_API UIG_ManagerBase : public UActorComponent
{
	GENERATED_BODY()

public:

protected:

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_ManagerBase", meta = (AllowPrivateAccess = true))
	TObjectPtr<AIG_PlayerController> playerController{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIG_ManagerBase", meta = (AllowPrivateAccess = true))
	TObjectPtr<AIGC_Player> playerCharacter{};

public:
	UIG_ManagerBase(const FObjectInitializer& _Init);

	FORCEINLINE TObjectPtr<AIG_PlayerController> GetPlayerController()const { return playerController; }
	FORCEINLINE TObjectPtr<AIGC_Player> GetPlayerCharacter()const { return playerCharacter; }

protected:
	void BeginPlay() override;

private:

};
