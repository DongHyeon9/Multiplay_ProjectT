#include "Lobby/Widget/LW_MatchWait.h"
#include "Lobby/Widget/LW_ConnectState.h"
#include "Lobby/L_PlayerController.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"

void ULW_MatchWait::NativeConstruct()
{
	Super::NativeConstruct();

	btn_Cancel->OnClicked.AddDynamic(this, &ULW_MatchWait::OnClickedCancel);
	connectStateList.Reserve(MAX_PLAYER_COUNT);
	APlayerController* owner{ GetOwningPlayer() };

	for (int32 i = 0; i < MAX_PLAYER_COUNT; ++i)
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
		connectState->SetState();
		connectStateList.Emplace(connectState);
	}

	refreshHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ULW_MatchWait::RefreshMatchState),
		refreshInterval
	);
}

void ULW_MatchWait::RemoveFromParent()
{
	FTSTicker::GetCoreTicker().RemoveTicker(refreshHandle);

	Super::RemoveFromParent();
}

bool ULW_MatchWait::RefreshMatchState(float _DeltaTime)
{
	auto gs = GetWorld()->GetGameState<AGameState>();
	for (int32 i = 0; i < connectStateList.Num(); ++i)
	{
		if (i < gs->PlayerArray.Num())
		{
			connectStateList[i]->SetState(gs->PlayerArray[i]);
		}
		else
		{
			connectStateList[i]->SetState();
		}
	}

	return true;
}

void ULW_MatchWait::OnClickedCancel()
{
	UGameplayStatics::OpenLevel(GetOwningPlayer(), TEXT("LV_Intro"));
}
