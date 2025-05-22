#include "Lobby/L_PlayerController.h"
#include "Lobby/Widget/LW_MatchWait.h"
#include "Lobby/L_GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "T_GameInstance.h"

void AL_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority())
	{
		//현재 로컬에서 실행중이라면 위젯을 띄우고 로그인 완료 신호를 보낸다
		matchWaitWidget = CreateWidget<ULW_MatchWait>(this, matchWaitClass);
		matchWaitWidget->AddToViewport();
		SetInputMode(FInputModeUIOnly{});
		SetShowMouseCursor(true);
		Server_SetPlayerName(GetGameInstance<UT_GameInstance>()->GetUserName());
	}
}

void AL_PlayerController::Server_SetPlayerName_Implementation(const FString& _UserName)
{
	//로그인 성공시 GM에 알려준다
	PTT_LOG(Warning, TEXT("%s : Login Success"), *_UserName);
	GetPlayerState<APlayerState>()->SetPlayerName(_UserName);
	if (auto gm = GetWorld()->GetAuthGameMode<AL_GameMode>())
	{
		gm->OnLoginComplete();
	}
}

bool AL_PlayerController::Server_SetPlayerName_Validate(const FString& _UserName)
{
	//프리픽스로 정상적인 이름인지 검사
	PTT_LOG(Warning, TEXT("%s"), *_UserName);
	return _UserName.StartsWith(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive);
}
