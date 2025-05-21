#pragma once

#include "IG_Enum.h"
#include "IG_Struct.generated.h"

USTRUCT(BlueprintType, Atomic)
struct FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "FCharacterData")
	FString characterName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "FCharacterData")
	FColor characterColor{};
};