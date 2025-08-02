#include "T_GameInstance.h"

void UT_GameInstance::SetUserName(const FString& _UserName)
{
	//서버에서 검증용으로 사용하기위해 포스트픽스를 적용
	userName = _UserName + PLAYER_NAME_POST_FIX;
	PTT_LOG(Warning, TEXT("User Name : %s"), *userName);
}
