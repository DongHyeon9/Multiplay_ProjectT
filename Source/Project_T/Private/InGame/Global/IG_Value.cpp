#include "InGame/Global/IG_Value.h"

const FName SESSION_NAME{ TEXT("ProjectTSession") };
const FName INTRO_LEVEL_NAME{ TEXT("LV_Intro") };
const FString PLAYER_NAME_POST_FIX{ TEXT(".PNPF") };

const int32 MAX_PLAYER_COUNT{ 2 };

const int32 PLAYER_MIN_LEVEL{ 1 };
const int32 PLAYER_MAX_LEVEL{ 15 };

const int32 SKILL_MIN_LEVEL{ 1 };
const int32 SKILL_MAX_LEVEL{ 5 };
const int32 SKILL_MAX_REGIST_COUNT{ 3 };

const uint8 SELECT_WIDGET_MAX_COUNT{ 3 };

const FName IG_COLLISION_TAG::CHARACTER{ TEXT("Character") };
const FName IG_COLLISION_TAG::ATTACK{ TEXT("Attack") };
const FName IG_COLLISION_TAG::DETECTION{ TEXT("Detection") };