#include "InGame/IG_GameState.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/IG_GameMode.h"
#include "Net/UnrealNetwork.h"

void AIG_GameState::OnCompletePlayer()
{
	++compeletedPlayer;
	PTT_LOG(Warning, TEXT("compeletedPlayer : %d"), compeletedPlayer.load());
	if (compeletedPlayer == MAX_PLAYER_COUNT)
	{
		compeletedPlayer = 0;
		UWorld* world = GetWorld();
		check(world);

		auto gm = world->GetAuthGameMode<AIG_GameMode>();
		check(gm);
		gm->OnCompleteAllPlayer();

		for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
		{
			if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
			{
				pc->Client_StartEvent();
			}
		}
	}
}

void AIG_GameState::StartGame()
{
	++compeletedPlayer;
	PTT_LOG(Warning, TEXT("compeletedPlayer : %d"), compeletedPlayer.load());
	if (compeletedPlayer == MAX_PLAYER_COUNT)
	{
		compeletedPlayer = 0;
		currentTime = gameTime;

		for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
		{
			if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
			{
				pc->Client_StartGame();
			}
		}

		gameTimerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::GameTimer), 1.0);
	}
}

void AIG_GameState::EndGame()
{
	PTT_LOG(Warning, TEXT(""));
	for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
		{
			pc->Client_EndEvent();
		}
	}
}

void AIG_GameState::ComputeResult()
{
	++compeletedPlayer;
	PTT_LOG(Warning, TEXT("compeletedPlayer : %d"), compeletedPlayer.load());
	if (compeletedPlayer == MAX_PLAYER_COUNT)
	{
		compeletedPlayer = 0;

		for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
		{
			if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
			{
				pc->Client_EndGame();
			}
		}
	}
}

bool AIG_GameState::GameTimer(float _DeltaTime)
{
	--currentTime;
	PTT_LOG(Warning, TEXT("%.1f"), currentTime);
	bool bIsEnd{ currentTime - KINDA_SMALL_NUMBER <= 0.0f };
	if (bIsEnd)
	{
		EndGame();
	}
	return !bIsEnd;
}

void AIG_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_GameState, currentTime);
}
