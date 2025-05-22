#include "T_GameInstance.h"

void UT_GameInstance::SetUserName(const FString& _UserName)
{
	userName = PLAYER_NAME_PREFIX + _UserName;
}
