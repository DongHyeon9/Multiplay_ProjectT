#include "InGame/Player/IG_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/IG_GameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "InGame/Widget/IGW_Main.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"

AIG_PlayerController::AIG_PlayerController(const FObjectInitializer& _Initializer):
	Super(_Initializer)
{
	playerMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("PlayerMappingContext"));
}

void AIG_PlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//playerMappingContext 설정하는 로직
}

void AIG_PlayerController::Server_InitPlayer_Implementation(const FString& _NewName)
{
	PTT_LOG(Warning, TEXT("%s : %s"), *GetName(), *_NewName);
	
	if (auto player = GetPawn<AIGC_Player>())
		player->SetCharacterName(_NewName);

	if (auto ps = GetPlayerState<APlayerState>())
		ps->SetPlayerName(_NewName.RightChop(PLAYER_NAME_PREFIX.Len()));

	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->OnCompletePlayer();
}

bool AIG_PlayerController::Server_InitPlayer_Validate(const FString& _NewName)
{
	PTT_LOG(Warning, TEXT("%s : %s"), *GetName(), *_NewName);

	return _NewName.StartsWith(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive);
}

void AIG_PlayerController::Server_OnFinishStartEvent_Implementation()
{
	PTT_LOG(Warning, TEXT("%s"), *GetName());

	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->RequestStartGame();

}

void AIG_PlayerController::Client_StartGame_Implementation()
{
	EnableInput(this);
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		TActorRange<AIG_CharacterBase> characters(GetWorld());
		for (auto character : characters)
		{
			if (character != GetPawn())
				character->GetStatusWidget()->SetHiddenInGame(false);
		}
	}

	if (mainWidget && mainWidget->IsInViewport())
	{
		mainWidget->InitTimer(GetWorld()->GetGameState<AIG_GameState>());
	}
}

void AIG_PlayerController::Client_StartEvent_Implementation()
{
	if (mainWidget && mainWidget->IsInViewport())
	{
		mainWidget->InitWidget();
		mainWidget->StartGame();
	}
}

void AIG_PlayerController::Client_EndEvent_Implementation()
{
	if (mainWidget && mainWidget->IsInViewport())
	{
		mainWidget->EndGame();
	}
}

void AIG_PlayerController::BeginPlay()
{
	Super::BeginPlay();
#if !WITH_EDITOR
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
#endif
		SetInputMode(FInputModeGameOnly{});
		SetShowMouseCursor(false);

		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			subsystem->AddMappingContext(playerMappingContext, 0);
		}
#if !WITH_EDITOR
		DisableInput(this);
#endif

		mainWidget = CreateWidget<UIGW_Main>(this, mainWidgetClass);
		mainWidget->AddToViewport();
		mainWidget->onFinishStartAnim.BindDynamic(this, &AIG_PlayerController::Server_OnFinishStartEvent);
		mainWidget->onFinishEndAnim.BindDynamic(this, &AIG_PlayerController::PrintResult);
#if !WITH_EDITOR
	}
#endif

#if WITH_EDITOR
	if (mainWidget && mainWidget->IsInViewport())
	{
		mainWidget->InitWidget();
		mainWidget->InitTimer(GetWorld()->GetGameState<AIG_GameState>());
	}
#endif
}

void AIG_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AIG_PlayerController::PrintResult()
{
	// TODO
	// 게임 종료시 처리해야될 로직
	// 결과창 UI
	auto gs = GetWorld()->GetGameState<AIG_GameState>();
	for (auto ps : gs->PlayerArray) 
	{
		PRINTSTR(-1, 10.0f, FColor::Green, TEXT("%s : %.1f"), *ps->GetPlayerName(), ps->GetScore());
	}
}
