#include "InGame/IG_GameMode.h"
#include "InGame/Player/IG_PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "InGame/IG_GameState.h"

namespace INGAME_GAME_MODE
{
	static constexpr const TCHAR* PLAYER_CONTROLLER_PATH{ TEXT("/Game/01_Blueprint/InGame/Player/BP_IG_PlayerController") };
	static constexpr const TCHAR* GAME_STATE_PATH{ TEXT("/Game/01_Blueprint/InGame/BP_IG_GameState") };
	static constexpr const TCHAR* PAWN_PATH{ TEXT("/Game/01_Blueprint/InGame/Player/BP_IGC_Player") };

	static constexpr const TCHAR* START_PREFIX{ TEXT("PlayerStart_") };
}


AIG_GameMode::AIG_GameMode(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> PLAYER_CONTROLLER{ INGAME_GAME_MODE::PLAYER_CONTROLLER_PATH };
	static ConstructorHelpers::FClassFinder<AGameState> GAME_STATE{ INGAME_GAME_MODE::GAME_STATE_PATH };
	static ConstructorHelpers::FClassFinder<APawn> PAWN{ INGAME_GAME_MODE::PAWN_PATH };

	if (PLAYER_CONTROLLER.Succeeded()) PlayerControllerClass = PLAYER_CONTROLLER.Class;
	if (GAME_STATE.Succeeded()) GameStateClass = GAME_STATE.Class;
	if (PAWN.Succeeded()) DefaultPawnClass = PAWN.Class;
}

void AIG_GameMode::OnEnterPlayer()
{
	SERVER_ONLY();

	auto pIter = GetWorld()->GetPlayerControllerIterator();
	for (int32 i = 0; i < MAX_PLAYER_COUNT; ++i, ++pIter)
	{
		// 모든 플레이어가 접속이 완료되면
		// 플레이어의 정보, 위치등의 정보를 초기화한다
		auto pc = Cast<AIG_PlayerController>(pIter->Get());
		if (pc)
		{
			auto ps = pc->GetPlayerState<APlayerState>();
			if (ps)
			{
				auto start = FindPlayerStart(pc, FString::Printf(TEXT("%s%d"), INGAME_GAME_MODE::START_PREFIX, i));
				if (start)
				{
					FPlayerData data{};
					data.playerName = ps->GetPlayerName();
					data.playerColor = playerColor.IsValidIndex(i) ? playerColor[i] : defaultColor;
					pc->InitPlayer(data);
					pc->GetPawn()->SetActorTransform(start->GetActorTransform());
				}
				else { PTT_LOG(Error, TEXT("Player Start Is Null")); }
			}
			else { PTT_LOG(Error, TEXT("APlayerState Is Null")); }
		}
		else { PTT_LOG(Error, TEXT("AIG_PlayerController Is Null")); }
	}
}
