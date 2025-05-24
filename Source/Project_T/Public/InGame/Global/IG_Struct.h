#pragma once

#include "IG_Enum.h"
#include "IG_Struct.generated.h"

USTRUCT(BlueprintType, Atomic)
struct FPlayerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "FPlayerData")
	FString playerName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "FPlayerData")
	FColor playerColor{};

	FString ToString()const
	{
		return FString::Printf(TEXT("%s : %s"), *playerName, *playerColor.ToString());
	}
};