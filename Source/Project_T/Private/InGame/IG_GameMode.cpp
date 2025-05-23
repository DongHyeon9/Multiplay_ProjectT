#include "InGame/IG_GameMode.h"
#include "InGame/Player/IG_PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "InGame/IG_GameState.h"

static const FString START_PREFIX{ TEXT("PlayerStart_") };

AIG_GameMode::AIG_GameMode(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> PLAYER_CONTROLLER(TEXT("/Game/01_Blueprint/InGame/Player/BP_IG_PlayerController"));
	static ConstructorHelpers::FClassFinder<AGameState> GAME_STATE(TEXT("/Game/01_Blueprint/InGame/BP_IG_GameState"));
	static ConstructorHelpers::FClassFinder<APawn> PAWN(TEXT("/Game/01_Blueprint/InGame/Player/BP_IGC_Player"));

	if (PLAYER_CONTROLLER.Succeeded()) PlayerControllerClass = PLAYER_CONTROLLER.Class;
	if (GAME_STATE.Succeeded()) GameStateClass = GAME_STATE.Class;
	if (PAWN.Succeeded()) DefaultPawnClass = PAWN.Class;
}

void AIG_GameMode::OnCompleteAllPlayer()
{
	auto pIter = GetWorld()->GetPlayerControllerIterator();
	for (int32 i = 0; i < MAX_PLAYER_COUNT; ++i,++pIter)
	{
		// 모든 플레이어가 접속이 완료되면
		// 플레이어의 정보, 위치등의 정보를 초기화한다
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
