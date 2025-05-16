#include "Lobby/Widget/LW_ConnectState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

#define LOCTEXT_NAMESPACE "ULW_ConnectState"

void ULW_ConnectState::SetState(const APlayerController* _PlayerController)
{
	FSlateBrush brush = img_PlayerImage->GetBrush();

	if (_PlayerController)
	{
		if (APlayerState* ps = _PlayerController->GetPlayerState<APlayerState>())
		{
			txt_PlayerName->SetText(FText::FromString(ps->GetPlayerName()));
			brush.TintColor = FColor::FromHex(TEXT("00FFFFB3"));
		}
	}
	else
	{
		txt_PlayerName->SetText(LOCTEXT("WatingConnect", "접속 대기중..."));
		brush.TintColor = FColor::FromHex(TEXT("00FFFF4D"));
	}
	img_PlayerImage->SetBrush(brush);
}

#undef LOCTEXT_NAMESPACE