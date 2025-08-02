#pragma once

#include "IG_Struct.h"

class AIGC_Enemy;
class AIG_SkillActorBase;

DECLARE_DELEGATE_TwoParams(FOnEnemyStateDelegate, AIGC_Enemy*, E_CHARACTER_STATE);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStateChangeDelegate, E_CHARACTER_STATE/*Prev*/, E_CHARACTER_STATE/*Current*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChangeDelegate, float/*Prev*/, float/*Current*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEXPChangeDelegate, float/*Prev*/, float/*Current*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChangeDelegate, int32/*Prev*/, int32/*Current*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDataChangeDelegate, const FCharacterData*/*Prev*/, const FCharacterData*/*Current*/);
DECLARE_DELEGATE_OneParam(FOnSelectedSkillDelegate, FName);
