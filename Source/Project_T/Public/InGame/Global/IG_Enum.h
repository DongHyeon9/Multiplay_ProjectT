#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class E_CHARACTER_STATE : uint8
{
	DISABLE = 0		UMETA(DisplayName = "Disable"),
	ENABLE			UMETA(DisplayName = "Enable"),
	DEAD			UMETA(DisplayName = "Dead"),

	END				UMETA(Hidden),
};
