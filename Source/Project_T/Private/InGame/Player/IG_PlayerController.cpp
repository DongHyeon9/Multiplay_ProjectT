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

void AIG_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_PlayerController, userName);
}

void AIG_PlayerController::Server_InitPlayer_Implementation(const FString& _NewName)
{
	userName = _NewName;

	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->OnCompletePlayer();
}

bool AIG_PlayerController::Server_InitPlayer_Validate(const FString& _NewName)
{
	return _NewName.StartsWith(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive);
}

void AIG_PlayerController::Server_StartEventEnd_Implementation()
{
	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->StartGame();
}

void AIG_PlayerController::Client_StartGame_Implementation()
{
	EnableInput(this);
	if (auto player = GetPawn<AIGC_Player>())
		player->GetStatusWidget()->SetHiddenInGame(false);
}

void AIG_PlayerController::Client_StartEvent_Implementation()
{
	if (mainWidget && mainWidget->IsInViewport())
	{

	}
	// TODO
	// 시작 이벤트 UI
	// 시작 이벤트 UI종료시 AIG_GameState::StartGame 호출
}

void AIG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		SetInputMode(FInputModeGameOnly{});
		SetShowMouseCursor(false);

		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			subsystem->AddMappingContext(playerMappingContext, 0);
		}
		DisableInput(this);

		mainWidget = CreateWidget<UIGW_Main>(this, mainWidgetClass);
		mainWidget->AddToViewport();
	}
}

void AIG_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AIG_PlayerController::OnRep_UpdateName()
{
	if (auto player = GetPawn<AIGC_Player>())
		player->SetUserName(userName);
}
