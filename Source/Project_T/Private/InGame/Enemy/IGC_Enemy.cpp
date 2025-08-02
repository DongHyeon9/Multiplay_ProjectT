#include "InGame/Enemy/IGC_Enemy.h"
#include "InGame/Component/Enemy/IG_EnemyStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/Player/IGC_Player.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "InGame/Component/Enemy/IG_DamageComponent.h"
#include "InGame/Component/Enemy/IG_TraceComponent.h"
#include "InGame/Widget/IGW_CharacterStatus.h"

const FName AIGC_Enemy::CAPSULE_NAME{ TEXT("CapsuleComp") };
const FName AIGC_Enemy::MESH_NAME{ TEXT("MeshComp") };
const FName AIGC_Enemy::MOVEMENT_NAME{ TEXT("EnemyMovementComp") };
const FName AIGC_Enemy::DAMAGE_NAME{ TEXT("DamageComp") };
const FName AIGC_Enemy::TRACE_NAME{ TEXT("TraceComp") };

namespace INGAME_ENEMY
{
	static constexpr const TCHAR* STATUS_WIDGET_PATH{ TEXT("/Game/01_Blueprint/InGame/Widget/WG_IGW_CharacterStatus") };
	static constexpr const TCHAR* MESH_PATH{ TEXT("/Game/02_Mesh/SKM_Manny") };
	static constexpr const TCHAR* ANIM_PATH{ TEXT("/Game/06_Animation/ABP_Manny") };
	static constexpr const TCHAR* MESH_MATERIAL_INSTANCE_01_PATH{ TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_01") };
	static constexpr const TCHAR* MESH_MATERIAL_INSTANCE_02_PATH{ TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_02") };

	static constexpr const TCHAR* COLLISION_NAME{ TEXT("Enemy") };

	static const FVector DEFAULT_MESH_LOCATION{ FVector(0.0f, 0.0f, -88.0f) };
	static const FRotator DEFAULT_MESH_ROTATION{ FRotator(0.0f, -90.0f, 0.0f) };
}

AIGC_Enemy::AIGC_Enemy(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<UCapsuleComponent>(CAPSULE_NAME);
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(MESH_NAME);
	statComp = CreateDefaultSubobject<UIG_EnemyStatComponent>(UIG_Character::STAT_COMP_NAME);
	statusWidget = CreateDefaultSubobject<UWidgetComponent>(UIG_Character::STAT_WIDGET_NAME);
	enemyMovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(MOVEMENT_NAME);
	damageComp = CreateDefaultSubobject<UIG_DamageComponent>(DAMAGE_NAME);
	traceComp = CreateDefaultSubobject<UIG_TraceComponent>(TRACE_NAME);

	RootComponent = rootComp;
	rootComp->InitCapsuleSize(34.0f, 88.0f);
	rootComp->SetCollisionProfileName(INGAME_ENEMY::COLLISION_NAME);
	rootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	rootComp->ComponentTags.Emplace(IG_COLLISION_TAG::CHARACTER);

	enemyMovementComp->MaxSpeed = 400.0f;

	statusWidget->SetupAttachment(RootComponent);
	statusWidget->bHiddenInGame = true;
	statusWidget->SetDrawSize(FVector2D{ 100.0f, 100.0f });
	statusWidget->SetRelativeLocation(FVector{ 0.0f,0.0f,100.0f });
	statusWidget->SetWidgetSpace(EWidgetSpace::Screen);

	meshComp->SetupAttachment(RootComponent);
	meshComp->bHiddenInGame = true;
	meshComp->SetRelativeLocationAndRotation(INGAME_ENEMY::DEFAULT_MESH_LOCATION, INGAME_ENEMY::DEFAULT_MESH_ROTATION);

	AutoPossessAI = EAutoPossessAI::Spawned;

	FDamageCompParameter param{};
	param.ownerCollision = rootComp;
	damageComp->InitDamageComponent(param);

	traceComp->InitTraceComponent
	({
		AIGC_Player::StaticClass(),
	});

	static ConstructorHelpers::FClassFinder<UUserWidget> WG_STATUS{ INGAME_ENEMY::STATUS_WIDGET_PATH };
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ENEMY{ INGAME_ENEMY::MESH_PATH };
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_ENEMY{ INGAME_ENEMY::ANIM_PATH };
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_01{ INGAME_ENEMY::MESH_MATERIAL_INSTANCE_01_PATH };
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_02{ INGAME_ENEMY::MESH_MATERIAL_INSTANCE_02_PATH };

	if (WG_STATUS.Succeeded()) statusWidget->SetWidgetClass(WG_STATUS.Class);
	if (SK_ENEMY.Succeeded()) meshComp->SetSkeletalMesh(SK_ENEMY.Object);
	if (ABP_ENEMY.Succeeded()) meshComp->SetAnimInstanceClass(ABP_ENEMY.Class);
	if (MI_ENEMY_01.Succeeded()) meshComp->SetMaterial(0, MI_ENEMY_01.Object);
	if (MI_ENEMY_02.Succeeded()) meshComp->SetMaterial(1, MI_ENEMY_02.Object);
}

void AIGC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	statComp->onStateChange.AddUObject(this, &AIGC_Enemy::OnChangeState);

	if (HasAuthority())
	{
		// 서버측에선 연산을 해야되는것 위주로 로직을 작성
		// 모든 판단의 주최는 서버(보안이슈)
		// ex) 데미지 적용, 오버랩 체크, 타겟 설정 등
		changeStateTable[static_cast<int32>(E_CHARACTER_STATE::ENABLE)] = [this]() {OnChangeState_Server_Enable(); };
		changeStateTable[static_cast<int32>(E_CHARACTER_STATE::DISABLE)] = [this]() {OnChangeState_Server_Disable(); };
		changeStateTable[static_cast<int32>(E_CHARACTER_STATE::DEAD)] = [this]() {OnChangeState_Server_Dead(); };

		enemyMovementComp->SetActive(false);
		statComp->InitStat(0);
		statComp->SetCharacterState(E_CHARACTER_STATE::DISABLE);
		rootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		// 클라측에선 비주얼 적인 요소 위주로 로직을 작성
		// ex) HP위젯 변경, State에 따른 상태 변경
		changeStateTable[static_cast<int32>(E_CHARACTER_STATE::ENABLE)] = [this]() {OnChangeState_Client_Enable(); };
		changeStateTable[static_cast<int32>(E_CHARACTER_STATE::DISABLE)] = [this]() {OnChangeState_Client_Disable(); };
		changeStateTable[static_cast<int32>(E_CHARACTER_STATE::DEAD)] = [this]() {OnChangeState_Client_Dead(); };

		auto status{ Cast<UIGW_CharacterStatus>(statusWidget->GetWidget()) };
		check(status);
		status->InitCharacterStatus(statComp);
		if (!statComp->onHPChange.IsBoundToObject(this))
			statComp->onHPChange.AddUObject(this, &AIGC_Enemy::OnChangeHP);
	}

#if WITH_EDITOR
	RootComponent->SetHiddenInGame(false);

	auto status{ Cast<UIGW_CharacterStatus>(statusWidget->GetWidget()) };
	check(status);
	status->InitCharacterStatus(statComp);
	if (!statComp->onHPChange.IsBoundToObject(this))
		statComp->onHPChange.AddUObject(this, &AIGC_Enemy::OnChangeHP);
#endif
}

void AIGC_Enemy::ActiveEnemy()
{
	damageComp->ActiveDamageComp(true);
	traceComp->ActiveTraceComp(true);
	statComp->InitStat(FMath::RandRange(0, statComp->GetTableMax()));
}

UPawnMovementComponent* AIGC_Enemy::GetMovementComponent() const
{
	return enemyMovementComp;
}

TObjectPtr<UIG_StatComponent> AIGC_Enemy::GetStatComp() const
{
	return statComp;
}

void AIGC_Enemy::OnChangeState(E_CHARACTER_STATE _PrevState, E_CHARACTER_STATE _NewState)
{
	changeStateTable[static_cast<int32>(_NewState)]();
	onEnemyState.ExecuteIfBound(this, _NewState);
}

void AIGC_Enemy::OnChangeHP(float _PrevHP, float _CurrentHP)
{
	if (FMath::IsNearlyEqual(statComp->GetCharacterData()->maxHP, _CurrentHP) || FMath::IsNearlyZero(_CurrentHP))
		statusWidget->SetHiddenInGame(true);
	else
		statusWidget->SetHiddenInGame(false);
}

void AIGC_Enemy::OnChangeState_Server_Enable()
{
	SetNetDormancy(ENetDormancy::DORM_Awake);
	rootComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMovementComponent()->SetActive(true);

	// 렉돌 해제
	SetRagdoll(false);
#if WITH_EDITOR
	OnChangeState_Client_Enable();
#endif
}

void AIGC_Enemy::OnChangeState_Server_Disable()
{
	SetNetDormancy(ENetDormancy::DORM_DormantAll);
#if WITH_EDITOR
	OnChangeState_Client_Disable();
#endif
}

void AIGC_Enemy::OnChangeState_Server_Dead()
{
	enemyMovementComp->SetActive(false);
	damageComp->ActiveDamageComp(false);
	traceComp->ActiveTraceComp(false);
	rootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 렉돌 처리
	SetRagdoll(true);

	// 일정 시간 후에 적을 비활성화 시킨다
	if (disableHandle.IsValid()) return;
	GetWorld()->GetTimerManager().SetTimer(disableHandle, FTimerDelegate::CreateUObject(statComp, &UIG_EnemyStatComponent::SetCharacterState, E_CHARACTER_STATE::DISABLE), disableDelay, false);
#if WITH_EDITOR
	OnChangeState_Client_Dead();
#endif
}

void AIGC_Enemy::OnChangeState_Client_Enable()
{
	meshComp->SetHiddenInGame(false);
	rootComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	auto status{ Cast<UIGW_CharacterStatus>(statusWidget->GetWidget()) };
	check(status);
	status->SetMaxHP(statComp->GetCharacterData()->maxHP);

	// 렉돌 해제
	SetRagdoll(false);
}

void AIGC_Enemy::OnChangeState_Client_Disable()
{
	meshComp->SetHiddenInGame(true);
}

void AIGC_Enemy::OnChangeState_Client_Dead()
{
	rootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	statusWidget->SetHiddenInGame(true);

	// 렉돌 처리
	SetRagdoll(true);
}

void AIGC_Enemy::SetRagdoll(bool _ActiveRagdoll)
{
	if (_ActiveRagdoll)
	{
		meshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);

		meshComp->SetAllBodiesSimulatePhysics(true);
		meshComp->WakeAllRigidBodies();

		meshComp->SetEnableGravity(true);

		
		meshComp->AddImpulse(GetActorForwardVector() * -FMath::FRandRange(10000.0f, 5000.0f));
	}
	else
	{
		meshComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

		meshComp->SetAllBodiesSimulatePhysics(false);
		meshComp->PutAllRigidBodiesToSleep();

		meshComp->UpdateKinematicBonesToAnim(meshComp->GetComponentSpaceTransforms(), ETeleportType::TeleportPhysics, true);
		meshComp->GetAnimInstance()->UpdateAnimation(GetWorld()->GetDeltaSeconds(), false);
		meshComp->RefreshBoneTransforms();

		meshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		meshComp->SetRelativeLocationAndRotation(INGAME_ENEMY::DEFAULT_MESH_LOCATION, INGAME_ENEMY::DEFAULT_MESH_ROTATION);

		meshComp->SetEnableGravity(false);
	}
}
