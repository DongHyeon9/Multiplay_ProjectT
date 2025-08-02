#include "InGame/Player/IG_PlayerController.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/Component/Player/IG_SkillComponent.h"
#include "InGame/Component/PlayerController/IGM_Screen.h"
#include "InGame/Component/PlayerController/IGM_Widget.h"
#include "T_GameInstance.h"

#include "Net/UnrealNetwork.h"
#include "InGame/IG_GameState.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"

const FName AIG_PlayerController::WIDGET_MANAGER_NAME{ TEXT("WidgetManager") };
const FName AIG_PlayerController::SCREEN_MANAGER_NAME{ TEXT("ScreenManager") };

AIG_PlayerController::AIG_PlayerController(const FObjectInitializer& _Initializer) :
	Super(_Initializer)
{
	widgetManager = CreateDefaultSubobject<UIGM_Widget>(WIDGET_MANAGER_NAME);
	screenManager = CreateDefaultSubobject<UIGM_Screen>(SCREEN_MANAGER_NAME);
}

void AIG_PlayerController::InitPlayer(const FPlayerData& _NewData)
{
	SERVER_ONLY();
	// 캐릭터의 데이터를 초기화한다
	auto player = Cast<AIGC_Player>(GetPawn());
	check(player);
	player->SetCharacterData(_NewData);
}

void AIG_PlayerController::OnEnterPlayer()
{
	FString userName{ GetGameInstance<UT_GameInstance>()->GetUserName() };

	Server_OnEnterPlayer(userName);

	userName.RemoveFromEnd(PLAYER_NAME_POST_FIX, ESearchCase::CaseSensitive);
	widgetManager->MainWidget_SetName(userName);
}

void AIG_PlayerController::GameOver()
{
	SERVER_ONLY();

	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->OnGameOverPlayer(this);
}

void AIG_PlayerController::EndGame()
{
	// 게임 종료 로직
	Client_EndEvent();
	auto player{ GetPawn<AIGC_Player>() };
	PTT_ERRER(player);
	player->EndGame();
}

void AIG_PlayerController::Server_OnEnterPlayer_Implementation(const FString& _NewName)
{
	PTT_LOG(Warning, TEXT("%s : %s"), *GetName(), *_NewName);

	FString playerName{ _NewName };
	playerName.RemoveFromEnd(PLAYER_NAME_POST_FIX, ESearchCase::CaseSensitive);

	// 플레이어가 입장이 완료되면
	// 게임스테이트와
	// 플레이어 스테이트를 초기화한다
	if (auto ps = GetPlayerState<APlayerState>())
		ps->SetPlayerName(playerName);

	if (auto player = GetPawn<AIGC_Player>())
		player->InitPlayerData();

	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->OnEnterPlayer(this);
}

bool AIG_PlayerController::Server_OnEnterPlayer_Validate(const FString& _NewName)
{
	PTT_LOG(Warning, TEXT("%s : %s"), *GetName(), *_NewName);
	// 플레이어의 이름이 유효한지 검증한다
	return _NewName.EndsWith(PLAYER_NAME_POST_FIX, ESearchCase::CaseSensitive);
}

void AIG_PlayerController::Server_SkillAcquisition_Implementation(FName _Skill)
{
	// 스킬 배우기
	auto player{ GetPawn<AIGC_Player>() };
	PTT_ERRER(player);
	PTT_LOG(Warning, TEXT("%s SelectSkill : %s"), *GetName(), *_Skill.ToString());
	player->SkillAcquisition(_Skill);
}

bool AIG_PlayerController::Server_SkillAcquisition_Validate(FName _Skill)
{
	// 스킬 유효성 검사
	auto player{ GetPawn<AIGC_Player>() };
	PTT_ERRER(player, false);
	auto skillComp{ player->GetSkillComp() };
	PTT_ERRER(skillComp, false);

	return skillComp->IsValidSkill(_Skill);
}

void AIG_PlayerController::Server_OnFinishStartEvent_Implementation()
{
	PTT_LOG(Warning, TEXT("%s"), *GetName());
	//시작 이벤트가 종료됐으면 서버에 알려준다(동기화)
	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->RequestStartGame(this);
}

void AIG_PlayerController::Client_StartGame_Implementation()
{
	//플레이어의 입력을 활성화 시킨다
	SetInputMode(FInputModeGameOnly{});
	screenManager->InitScreenManager();
	TActorRange<AIGC_Player> players{ GetWorld() };
	// 클라에서 플레이어 초기화
	for (auto player : players)
	{
		player->InitPlayerData();
	}
}

void AIG_PlayerController::Client_StartEvent_Implementation()
{
	widgetManager->MainWidget_StartGameAnim();
}

void AIG_PlayerController::Client_EndEvent_Implementation()
{
	widgetManager->MainWidget_EndGameAnim();
}

void AIG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	
#if !WITH_EDITOR
	if (!HasAuthority())
#endif
	{
		widgetManager->MainWidget_GetOnFinishStartAnim().BindDynamic(this, &AIG_PlayerController::Server_OnFinishStartEvent);
		widgetManager->MainWidget_Init();
	}
}