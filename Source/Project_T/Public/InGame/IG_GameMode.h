#pragma once

#include "../Project_T.h"
#include "GameFramework/GameMode.h"
#include "IG_GameMode.generated.h"

UCLASS()
class PROJECT_T_API AIG_GameMode : public AGameMode
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameMode", meta = (AllowPrivateAccess = true))
	TArray<FColor> playerColor{ FColor::Red, FColor::Blue, FColor::Green };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameMode", meta = (AllowPrivateAccess = true))
	FColor defaultColor{ FColor::White };

public:
	AIG_GameMode(const FObjectInitializer& _Initializer);
	void OnCompleteAllPlayer();
};
