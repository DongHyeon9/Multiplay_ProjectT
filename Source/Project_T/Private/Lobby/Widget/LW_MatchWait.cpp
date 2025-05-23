﻿#include "Lobby/Widget/LW_MatchWait.h"
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

ULW_MatchWait::ULW_MatchWait(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> CONNECT_STATE_WIDGET(TEXT("/Game/01_Blueprint/Lobby/Widget/WG_L_ConnectState"));

	if (CONNECT_STATE_WIDGET.Succeeded())connectStateClass = CONNECT_STATE_WIDGET.Class;
}

void ULW_MatchWait::NativeConstruct()
{
	Super::NativeConstruct();

	btn_Cancel->OnClicked.AddDynamic(this, &ULW_MatchWait::OnClickedCancel);
	connectStateList.Reserve(MAX_PLAYER_COUNT);
	APlayerController* owner{ GetOwningPlayer() };

	//최대 플레이어의 수 만큼 connectState위젯을 생성한다
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

	PTT_LOG(Warning, TEXT("%s : Main Widget Init Success!"), *GetOwningPlayer()->GetName());
}

void ULW_MatchWait::RemoveFromParent()
{
	if (refreshHandle.IsValid())
		FTSTicker::GetCoreTicker().RemoveTicker(refreshHandle), refreshHandle.Reset();

	Super::RemoveFromParent();
}

bool ULW_MatchWait::RefreshMatchState(float _DeltaTime)
{
	//현재 플레이어들의 접속상태를 갱신한다
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
	// 취소를 누를 시 원래 있던 레벨로 돌아간다
	UGameplayStatics::OpenLevel(GetOwningPlayer(), TEXT("LV_Intro"));
}
