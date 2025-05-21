#include "InGame/IG_GameMode.h"
#include "InGame/Player/IG_PlayerController.h"
#include "GameFramework/PlayerState.h"

static const FString START_PREFIX{ TEXT("PlayerStart_") };

void AIG_GameMode::OnCompleteAllPlayer()
{
	auto pIter = GetWorld()->GetPlayerControllerIterator();
	for (int32 i = 0; i < MAX_PLAYER_COUNT; ++i,++pIter)
	{
		auto pc = Cast<AIG_PlayerController>(pIter->Get());
		check(pc);
		auto ps = pc->GetPlayerState<APlayerState>();
		check(ps);
		auto start = FindPlayerStart(pc, FString::Printf(TEXT("%s%d"), *START_PREFIX, i));
		FCharacterData data{};
		data.characterName = ps->GetPlayerName();
		data.characterColor = playerColor.Num() > i ? playerColor[i] : defaultColor;
		pc->InitPlayer(data);
		pc->GetPawn()->SetActorTransform(start->GetActorTransform());
	}
}
