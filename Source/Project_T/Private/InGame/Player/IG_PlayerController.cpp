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

#include "DrawDebugHelpers.h"

AIG_PlayerController::AIG_PlayerController(const FObjectInitializer& _Initializer):
	Super(_Initializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MAIN_WIDGET(TEXT("/Game/01_Blueprint/InGame/Widget/WG_IGW_Main"));

	if (MAIN_WIDGET.Succeeded()) mainWidgetClass = MAIN_WIDGET.Class;
}

void AIG_PlayerController::InitPlayer(const FPlayerData& _NewData)
{
	//캐릭터의 데이터를 초기화한다
	auto character = GetPawn<AIGC_Player>();
	if (character)
		character->SetCharacterData(_NewData);
}

void AIG_PlayerController::Server_InitPlayer_Implementation(const FString& _NewName)
{
	PTT_LOG(Warning, TEXT("%s : %s"), *GetName(), *_NewName);
	
	// 플레이어가 입장이 완료되면
	// 게임스테이트와
	// 플레이어 스테이트를 초기화한다
	if (auto ps = GetPlayerState<APlayerState>())
		ps->SetPlayerName(_NewName.RightChop(PLAYER_NAME_PREFIX.Len()));

	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->OnInitPlayer();
}

bool AIG_PlayerController::Server_InitPlayer_Validate(const FString& _NewName)
{
	PTT_LOG(Warning, TEXT("%s : %s"), *GetName(), *_NewName);
	// 플레이어의 이름이 유효한지 검증한다
	return _NewName.StartsWith(PLAYER_NAME_PREFIX, ESearchCase::CaseSensitive);
}

void AIG_PlayerController::Server_SendScreenInfo_Implementation(const FVector& _Center, const float _Radius)
{
	//Unreliable이라 누락된 데이터는 반영하지 않는다
	if (_Radius < KINDA_SMALL_NUMBER) return;
	screenInfo = TPair<FVector, float>{ _Center, _Radius };
}

void AIG_PlayerController::Server_OnFinishStartEvent_Implementation()
{
	PTT_LOG(Warning, TEXT("%s"), *GetName());
	//시작 이벤트가 종료됐으면 서버에 알려준다(동기화)
	if (auto gs = GetWorld()->GetGameState<AIG_GameState>())
		gs->RequestStartGame();
}

void AIG_PlayerController::Client_StartGame_Implementation()
{
	//플레이어의 입력을 활성화 시킨다
	EnableInput(this);
	TActorRange<AIGC_Player> players(GetWorld());
	for (auto player : players)
	{
		//내 캐릭터 외에 다른 플레이어의 체력바를 보여준다
		if (player != GetPawn())
			player->GetStatusWidget()->SetHiddenInGame(false);
	}
	//타이머를 활성화한다
	if (mainWidget && mainWidget->IsInViewport())
		mainWidget->InitTimer(GetWorld()->GetGameState<AIG_GameState>());
}

void AIG_PlayerController::Client_StartEvent_Implementation()
{
	//게임시작 애니메이션을 재생한다
	mainWidget->StartGame();
}

void AIG_PlayerController::Client_EndEvent_Implementation()
{
	//게임 종료 애니메이션을 재생한다
	mainWidget->EndGame();
}

void AIG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 인풋 모드를 변경한다
	SetInputMode(FInputModeGameOnly{});
	SetShowMouseCursor(false);

	//디버깅을 위해 로직을 분리한다
#if WITH_EDITOR
	mainWidget = CreateWidget<UIGW_Main>(this, mainWidgetClass);
	mainWidget->AddToViewport();
	mainWidget->onFinishStartAnim.BindDynamic(this, &AIG_PlayerController::Server_OnFinishStartEvent);
	mainWidget->onFinishEndAnim.BindDynamic(this, &AIG_PlayerController::PrintResult);
	mainWidget->InitWidget();
	mainWidget->InitTimer(GetWorld()->GetGameState<AIG_GameState>());
#endif

#if !WITH_EDITOR
	// 위젯을 생성하고 초기화한다
	if (!HasAuthority())
	{
		mainWidget = CreateWidget<UIGW_Main>(this, mainWidgetClass);
		mainWidget->AddToViewport();
		mainWidget->onFinishStartAnim.BindDynamic(this, &AIG_PlayerController::Server_OnFinishStartEvent);
		mainWidget->onFinishEndAnim.BindDynamic(this, &AIG_PlayerController::PrintResult);
		mainWidget->InitWidget();
		mainWidget->InitTimer(GetWorld()->GetGameState<AIG_GameState>());
	}
	//인풋을 비활성화하고
	DisableInput(this);
#endif
}

void AIG_PlayerController::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	const float traceDistance{ 10000.0f };
	UWorld* world{ GetWorld() };
	FHitResult outResult{};

	int32 sizeX{}, sizeY{};
	GetViewportSize(sizeX, sizeY);

	//스크린 중앙의 월드좌표를 구한다
	FVector worldLocationM{}, worldDirectionM{};
	DeprojectScreenPositionToWorld(sizeX * 0.5, sizeY * 0.5, worldLocationM, worldDirectionM);
	FVector targetLocation{ worldDirectionM * traceDistance + worldLocationM };
	if (world->LineTraceSingleByObjectType(
		outResult,
		worldLocationM,
		targetLocation,
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
	{
		worldLocationM = outResult.ImpactPoint;
	}

	//스크린 좌측 상단의 월드좌표를 구한다
	FVector worldLocationTL{}, worldDirectionTL{};
	DeprojectScreenPositionToWorld(0, 0, worldLocationTL, worldDirectionTL);
	targetLocation = worldDirectionTL * traceDistance + worldLocationTL;
	if (world->LineTraceSingleByObjectType(
		outResult,
		worldLocationTL,
		targetLocation,
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
	{
		worldLocationTL = outResult.ImpactPoint;
	}

	//반지름과 중앙좌표를 서버로 보낸다
	float radius{ static_cast<float>(abs((worldLocationM - worldLocationTL).Length())) };
	Server_SendScreenInfo(worldLocationM, radius);
}

void AIG_PlayerController::PrintResult()
{
	auto gs = GetWorld()->GetGameState<AIG_GameState>();
	for (auto ps : gs->PlayerArray) 
	{
		PRINTSTR(-1, 10.0f, FColor::Green, TEXT("%s : %.1f"), *ps->GetPlayerName(), ps->GetScore());
		// TODO
		// 게임 종료시 처리해야될 로직
		// 결과창 UI
	}
}
