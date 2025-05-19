#include "InGame/IG_GameState.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/IG_GameMode.h"
#include "Net/UnrealNetwork.h"

void AIG_GameState::OnCompletePlayer()
{
	++compeletedPlayer;
	if (compeletedPlayer == MAX_PLAYER_COUNT)
	{
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
		compeletedPlayer = 0;
	}
}

void AIG_GameState::StartGame()
{
	++compeletedPlayer;
	if (compeletedPlayer == MAX_PLAYER_COUNT)
	{
		currentTime = gameTime;

		for (auto pIter = GetWorld()->GetPlayerControllerIterator(); pIter; ++pIter)
		{
			if (auto pc = Cast<AIG_PlayerController>(pIter->Get()))
			{
				pc->Client_StartGame();
			}
		}

		compeletedPlayer = 0;
		gameTimerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIG_GameState::GameTimer));
	}
}

void AIG_GameState::EndGame()
{
	// TODO
	// 게임 종료 로직
	// 결과창 UI
}

bool AIG_GameState::GameTimer(float _DeltaTime)
{
	currentTime -= _DeltaTime;
	bool bIsEnd{ currentTime <= 0.0f };
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
