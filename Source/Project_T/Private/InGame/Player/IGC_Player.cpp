#include "InGame/Player/IGC_Player.h"
#include "InGame/Component/Player/IG_SkillComponent.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/Widget/IGW_CharacterStatus.h"
#include "InGame/Component/Player/IG_PlayerStatComponent.h"
#include "InGame/Widget/IGW_Main.h"
#include "InGame/Component/PlayerController/IGM_Widget.h"
#include "InGame/Component/PlayerController/IGM_Screen.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Net/UnrealNetwork.h"

#define SET_MESH_COLOR(idx) 	auto mat = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(idx));				\
								if (mat) mat->SetVectorParameterValue(TEXT("Tint"), playerData.playerColor)
#define CREATE_DYNAMIC_MATERIAL(idx) GetMesh()->SetMaterial(idx, UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(idx), this))

const FName AIGC_Player::CAMERA_NAME{ TEXT("CameraComp") };
const FName AIGC_Player::SPRINGARM_NAME{ TEXT("SpringArmComp") };
const FName AIGC_Player::SKILL_COMP_NAME{ TEXT("SkillComp") };

namespace INGAME_PLAYER
{
	static constexpr const TCHAR* MESH_PATH{ TEXT("/Game/02_Mesh/SKM_Manny") };
	static constexpr const TCHAR* ANIM_PATH{ TEXT("/Game/06_Animation/ABP_Manny") };
	static constexpr const TCHAR* STATUS_PATH{ TEXT("/Game/01_Blueprint/InGame/Widget/WG_IGW_CharacterStatus") };
}

AIGC_Player::AIGC_Player(const FObjectInitializer& _Intializer) :
	Super(_Intializer)
{
	PrimaryActorTick.bCanEverTick = true;

	statComp = CreateDefaultSubobject<UIG_PlayerStatComponent>(UIG_Character::STAT_COMP_NAME);
	statusWidget = CreateDefaultSubobject<UWidgetComponent>(UIG_Character::STAT_WIDGET_NAME);
	skillComp = CreateDefaultSubobject<UIG_SkillComponent>(SKILL_COMP_NAME);
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(SPRINGARM_NAME);
	cameraComp = CreateDefaultSubobject<UCameraComponent>(CAMERA_NAME);

	GetRootComponent()->ComponentTags.Emplace(IG_COLLISION_TAG::CHARACTER);

	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	springArmComp->TargetArmLength = 1200.0f;
	springArmComp->bEnableCameraLag = true;
	springArmComp->CameraLagSpeed = 3.0f;
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->bInheritRoll = true;
	springArmComp->bInheritPitch = false;
	springArmComp->bInheritYaw = true;
	springArmComp->bDoCollisionTest = false;
	bUseControllerRotationYaw = false;

	statusWidget->SetupAttachment(RootComponent);
	statusWidget->bHiddenInGame = true;
	statusWidget->SetDrawSize(FVector2D{ 250.0f, 250.0f });
	statusWidget->SetWidgetSpace(EWidgetSpace::Screen);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.f, 0.0f);

	cameraComp->SetupAttachment(springArmComp);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PLAYER{ INGAME_PLAYER::MESH_PATH };
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_PLAYER{ INGAME_PLAYER::ANIM_PATH };
	static ConstructorHelpers::FClassFinder<UUserWidget> WG_STATUS{ INGAME_PLAYER::STATUS_PATH };

	if (SK_PLAYER.Succeeded()) GetMesh()->SetSkeletalMesh(SK_PLAYER.Object);
	if (ABP_PLAYER.Succeeded()) GetMesh()->SetAnimInstanceClass(ABP_PLAYER.Class);
	if (WG_STATUS.Succeeded()) statusWidget->SetWidgetClass(WG_STATUS.Class);

	initTable[ENetRole::ROLE_Authority] = [this]() {InitPlayerData_Authority(); };
	initTable[ENetRole::ROLE_AutonomousProxy] = [this]() {InitPlayerData_AutonomousProxy(); };
	initTable[ENetRole::ROLE_SimulatedProxy] = [this]() {InitPlayerData_SimulatedProxy(); };
}

void AIGC_Player::SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(_PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(_PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AIGC_Player::Move);
	}
}

void AIGC_Player::InitPlayerData()
{
	// 플레이어 초기화 로직
	initTable[GetLocalRole()]();
}

void AIGC_Player::SetCharacterData(const FPlayerData& _NewData)
{
	PTT_LOG(Warning, TEXT("%s"), *_NewData.ToString());
	playerData = _NewData;
}

void AIGC_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIGC_Player, playerData);
}

void AIGC_Player::SkillAcquisition(FName _Skill)
{
	skillComp->RegistSkill(_Skill);
	bIsSkillSelectProcessing = false;
	if (skillComp->GetAcquisitionCount() > 0)
		SkillAcquisitionProcess();
}

void AIGC_Player::GameOver()
{
	auto pc{ GetController<AIG_PlayerController>() };
	PTT_ERRER(pc);
	pc->GameOver();
	
	EndGame();
}

void AIGC_Player::EndGame()
{
	// 게임 종료 로직
	skillComp->DestroySkills();
	GetMovementComponent()->SetActive(false);
	GetMovementComponent()->StopMovementImmediately();
}

TObjectPtr<UIG_StatComponent> AIGC_Player::GetStatComp() const
{
	return statComp;
}

void AIGC_Player::BeginPlay()
{
	Super::BeginPlay();

	if (!statComp->onStateChange.IsBoundToObject(this))
		statComp->onStateChange.AddUObject(this, &AIGC_Player::OnChangeState);

	if (HasAuthority())
	{
		if (!statComp->onLevelChange.IsBoundToObject(this))
			statComp->onLevelChange.AddUObject(this, &AIGC_Player::OnLevelUp);
	}
#if WITH_EDITOR
	if (AIG_PlayerController* pc = Cast<AIG_PlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
			subsystem->AddMappingContext(characterMappingContext, 0);

		FTimerHandle handle{};
		auto screenManager{ pc->GetScreenManager() };
		GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(screenManager, &UIGM_Screen::InitScreenManager), 0.5f, false);
	}

	RootComponent->SetHiddenInGame(false);
	InitPlayerData();
#else
	else
	{
		if (APlayerController* playerController = Cast<APlayerController>(Controller))
			if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
				subsystem->AddMappingContext(characterMappingContext, 0);
		//서버에 입장완료를 알린다
		if (auto pc = GetController<AIG_PlayerController>())
			pc->OnEnterPlayer();
		// 다이나믹 머티리얼을 생성한다
		int32 materialCount{ GetMesh()->GetMaterials().Num() };
		for (int32 i = 0; i < materialCount; ++i)
		{
			CREATE_DYNAMIC_MATERIAL(i);
		}
	}
#endif
}

void AIGC_Player::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//플레이어 입력을 설정해준다
	characterMappingContext = NewObject<UInputMappingContext>(this);
	moveAction = NewObject<UInputAction>(this);
	moveAction->ValueType = EInputActionValueType::Axis2D;
	auto swizzleAxis = NewObject<UInputModifierSwizzleAxis>(this);
	auto negate = NewObject<UInputModifierNegate>(this);

	characterMappingContext->MapKey(moveAction, EKeys::D);
	characterMappingContext->MapKey(moveAction, EKeys::A).Modifiers.Emplace(negate);
	characterMappingContext->MapKey(moveAction, EKeys::W).Modifiers.Emplace(swizzleAxis);
	auto& moveBackward = characterMappingContext->MapKey(moveAction, EKeys::S);
	moveBackward.Modifiers.Emplace(swizzleAxis);
	moveBackward.Modifiers.Emplace(negate);
}

void AIGC_Player::Move(const FInputActionValue& _Value)
{
	if (Controller == nullptr) return;

	FVector2D movementVector = _Value.Get<FVector2D>();
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), movementVector.Y);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), movementVector.X);
}

void AIGC_Player::OnRep_UpdatePlayerData()
{
	//서버에서 캐릭터의 정보가 변경되면 클라이언트에서 업데이트한다
	if (playerData.playerName.EndsWith(PLAYER_NAME_POST_FIX, ESearchCase::CaseSensitive))
		playerData.playerName.RemoveFromEnd(PLAYER_NAME_POST_FIX, ESearchCase::CaseSensitive);

	int32 materialCount{ GetMesh()->GetMaterials().Num() };
	for (int32 i = 0; i < materialCount; ++i)
	{
		SET_MESH_COLOR(i);
	}
}

void AIGC_Player::OnLevelUp(int32 _Prev, int32 _Current)
{
	skillComp->AddAcquisitionCount(_Current - _Prev);

	SkillAcquisitionProcess();
}

void AIGC_Player::InitPlayerData_SimulatedProxy()
{
	// 타 플레이어 초기화
	statusWidget->SetHiddenInGame(false);
	statComp->InitStat(PLAYER_MIN_LEVEL);
	auto status{ Cast<UIGW_CharacterStatus>(statusWidget->GetWidget()) };
	check(status);
	status->InitCharacterStatus(statComp, playerData.playerName);
}

void AIGC_Player::InitPlayerData_AutonomousProxy()
{
	// 클라이언트 초기화 로직
	if (auto pc{ GetController<AIG_PlayerController>() })
		pc->GetWidgetManager()->MainWidget_BindStat(statComp);
}

void AIGC_Player::InitPlayerData_Authority()
{
	// 서버 초기화 로직
	statComp->InitStat(PLAYER_MIN_LEVEL);
#if WITH_EDITOR
	if (auto pc{ GetController<AIG_PlayerController>() })
		pc->GetWidgetManager()->MainWidget_BindStat(statComp);
#endif
}

void AIGC_Player::SkillAcquisitionProcess()
{
	if (statComp->GetCharacterState() != E_CHARACTER_STATE::ENABLE) return;
	if (bIsSkillSelectProcessing) return;
	bIsSkillSelectProcessing = true;
	// 레벨업 시 진행할 로직
	// 1. 다음에 배울 스킬 산출
	TArray<FName> randomSkills{};
	skillComp->GetRandomSkills(randomSkills);

	// 2. 플레이어한테 UI전송
	auto pc{ GetController<AIG_PlayerController>() };
	PTT_ERRER(pc);
	auto wm{ pc->GetWidgetManager() };
	PTT_ERRER(wm);
	wm->MainWidget_Client_SkillAcquisition(randomSkills);
}

void AIGC_Player::OnChangeState(E_CHARACTER_STATE _Prev, E_CHARACTER_STATE _Current)
{
	if (_Current != E_CHARACTER_STATE::DEAD) return;

	// 렉돌 처리
	auto meshComp{ GetMesh() };

	meshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);

	meshComp->SetAllBodiesSimulatePhysics(true);
	meshComp->WakeAllRigidBodies();

	meshComp->SetEnableGravity(true);

	meshComp->AddImpulse(GetActorForwardVector() * -FMath::FRandRange(10000.0f, 5000.0f));
}

#undef SET_MESH_COLOR
#undef CREATE_DYNAMIC_MATERIAL