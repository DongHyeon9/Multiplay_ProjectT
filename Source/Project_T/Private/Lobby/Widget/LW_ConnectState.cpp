#include "Lobby/Widget/LW_ConnectState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetStringLibrary.h"

#define LOCTEXT_NAMESPACE "ULW_ConnectState"

void ULW_ConnectState::SetState(const APlayerState* _PlayerState)
{
	FSlateBrush brush = img_PlayerImage->GetBrush();

	if (_PlayerState)
	{
		// 현재 플레이어 이름이
		// 서버와 동기화된 상태라면 플레이어 이름을
		// 동기화 전이라면 접속중...이라고 표시한다
		FString playerName{ _PlayerState->GetPlayerName() };
		if (playerName.Contains(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive))
		{
			playerName = playerName.RightChop(PLAYER_NAME_PREFIX.Len());
		}
		else
		{
			playerName = TEXT("접속중...");
		}
		txt_PlayerName->SetText(FText::FromString(playerName));
		brush.TintColor = FColor::FromHex(TEXT("00FFFFB3"));
	}
	else
	{
		txt_PlayerName->SetText(LOCTEXT("WatingConnect", "접속 대기중..."));
		brush.TintColor = FColor::FromHex(TEXT("00FFFF4D"));
	}
	img_PlayerImage->SetBrush(brush);
}

#undef LOCTEXT_NAMESPACE