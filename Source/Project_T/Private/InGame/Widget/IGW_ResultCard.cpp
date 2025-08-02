#include "InGame/Widget/IGW_ResultCard.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

#define LOCTEXT_NAMESPACE "InGameResult"

void UIGW_ResultCard::InitResultCard(TObjectPtr<APlayerState> _PlayerState)
{
	txt_Score->SetText(FText::Format(
		LOCTEXT("ResultCard", "{0} : {1}"),
		{
			FText::FromString(_PlayerState->GetPlayerName()),
			FText::AsNumber(_PlayerState->GetScore())
		}));
}

#undef LOCTEXT_NAMESPACE