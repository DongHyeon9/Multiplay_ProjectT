#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class E_CHARACTER_STATE : uint8
{
	DISABLE		UMETA(DisplayName = "Disable"),
	ENABLE		UMETA(DisplayName = "Enable"),
	DEAD		UMETA(DisplayName = "Dead"),
};
