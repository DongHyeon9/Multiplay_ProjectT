#include "InGame/IG_GameMode.h"
#include "GameFramework/PlayerController.h"

static const FString START_PREFIX{ TEXT("PlayerStart_") };

void AIG_GameMode::OnCompleteAllPlayer()
{
	auto pIter = GetWorld()->GetPlayerControllerIterator();
	for (int32 i = 0; i < MAX_PLAYER_COUNT; ++i,++pIter)
	{
		auto pc = pIter->Get();
		auto start = FindPlayerStart(pc, FString::Printf(TEXT("%s%d"), *START_PREFIX, i));
		pc->GetPawn()->SetActorTransform(start->GetActorTransform());
	}
}
