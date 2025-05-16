#include "Lobby/Widget/LW_MatchWait.h"
#include "Lobby/Widget/LW_ConnectState.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Blueprint/WidgetTree.h"

void ULW_MatchWait::Init(int32 _MaxPlayerCount, const TArray<APlayerController*>& _PrevPlayers)
{
	connectStateMap.Reserve(_MaxPlayerCount);
	APlayerController* owner{ GetOwningPlayer() };

	for (int32 i = 0; i < _MaxPlayerCount; ++i)
	{
		USizeBox* sizeBox{ WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass()) };
		sizeBox->SetWidthOverride(connectStateSize);
		sizeBox->SetHeightOverride(connectStateSize);

		if (UHorizontalBoxSlot* hbSlot = hb_PlayerList->AddChildToHorizontalBox(sizeBox))
		{
			hbSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			hbSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
			hbSlot->SetSize(FSlateChildSize{});
		}

		ULW_ConnectState* connectState{ CreateWidget<ULW_ConnectState>(owner,connectStateClass) };
		sizeBox->AddChild(connectState);

		APlayerController* pc{ i < _PrevPlayers.Num() ? _PrevPlayers[i] : nullptr };
		connectState->SetState(pc);
		connectStateMap.Emplace(pc, connectState);
	}
}

void ULW_MatchWait::OnLoginPlayer(const APlayerController* _Player)
{
	check(!connectStateMap.Contains(_Player));
	connectStateMap[_Player]->SetState(_Player);
}

void ULW_MatchWait::OnLogoutPlayer(const APlayerController* _Player)
{
	check(connectStateMap.Contains(_Player));
	connectStateMap[_Player]->SetState();
}