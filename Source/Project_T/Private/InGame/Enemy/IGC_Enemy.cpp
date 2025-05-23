﻿#include "InGame/Enemy/IGC_Enemy.h"
#include "InGame/IG_StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/Player/IGC_Player.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <limits>

AIGC_Enemy::AIGC_Enemy(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	GetCharacterMovement()->bAutoActivate = false;

	bReplicates = true;
	SetReplicateMovement(true);

	enemyMovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("EnemyMovementComp"));
	enemyMovementComp->MaxSpeed = 400.0f;

	GetMesh()->bHiddenInGame = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AutoPossessAI = EAutoPossessAI::Spawned;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_01(TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_01"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_02(TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_02"));

	auto mesh = GetMesh();
	if (MI_ENEMY_01.Succeeded()) mesh->SetMaterial(0, MI_ENEMY_01.Object);
	if (MI_ENEMY_02.Succeeded()) mesh->SetMaterial(1, MI_ENEMY_02.Object);
}

void AIGC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->DestroyComponent();
	UCapsuleComponent* enemyCollision{ GetCapsuleComponent() };

	GetStatComp()->onChangeState.AddUObject(this, &AIGC_Enemy::Multicast_OnChangeState);
	enemyCollision->OnComponentBeginOverlap.AddDynamic(this, &AIGC_Enemy::OnBeginOverlap);
	enemyCollision->OnComponentEndOverlap.AddDynamic(this, &AIGC_Enemy::OnEndOverlap);
}

void AIGC_Enemy::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	if (disableHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(disableHandle), disableHandle.Invalidate();

	if (damageHandle.IsValid())
		FTSTicker::GetCoreTicker().RemoveTicker(damageHandle), damageHandle.Reset();
}

void AIGC_Enemy::InitEnemy()
{
	//초기 상태를 비활성화 상태로 만든다
	SetNetDormancy(ENetDormancy::DORM_DormantAll);
	GetStatComp()->SetCharacterState(E_CHARACTER_STATE::DISABLE);
}

void AIGC_Enemy::Multicast_SetActive_Implementation(bool _bIsActive)
{
	//적을 활성상태를 결정한다
	if (HasAuthority())
		SetNetDormancy(_bIsActive ? ENetDormancy::DORM_Awake : ENetDormancy::DORM_DormantAll);

	GetMesh()->SetHiddenInGame(!_bIsActive);
	if (_bIsActive)
		GetStatComp()->SetCharacterState(E_CHARACTER_STATE::ENABLE);
}

bool AIGC_Enemy::ApplyDamage(float _DeltaTime)
{
	for (auto player : overlappedPlayer)
	{
		PTT_LOG(Warning, TEXT("%s : Damage!"), *player->GetName());
		// TODO
		// 데미지 적용 로직
	}
	return true;
}

void AIGC_Enemy::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (!targetPlayer) return;

	FVector targetPos{ targetPlayer->GetActorLocation() };
	FVector curPos{ GetActorLocation() };
	FVector dir{ targetPos - curPos };
	dir.Normalize();

	AddMovementInput(dir);

	if (prevDir.Equals(dir)) return;

	prevDir = dir;
	SetActorRotation(prevDir.Rotation());
}

UPawnMovementComponent* AIGC_Enemy::GetMovementComponent() const
{
	return enemyMovementComp;
}

void AIGC_Enemy::Multicast_OnChangeState_Implementation(E_CHARACTER_STATE _NewState)
{
	switch (_NewState)
	{
	case E_CHARACTER_STATE::DISABLE: {
		Multicast_SetActive(false);
		break;
	}
	case E_CHARACTER_STATE::ENABLE: {
		// 적의 상태를 활성화 시킨다
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SetTarget();
		break;
	}
	case E_CHARACTER_STATE::DEAD: {
		RemoveTarget();
		GetStatusWidget()->SetHiddenInGame(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 일정 시간 후에 적을 비활성화 시킨다
		if (disableHandle.IsValid()) GetWorld()->GetTimerManager().ClearTimer(disableHandle), disableHandle.Invalidate();
		GetWorld()->GetTimerManager().SetTimer(disableHandle, FTimerDelegate::CreateUObject(this, &AIGC_Enemy::OnDelay_ChangeDisable), disableDelay, false);
		break;
	}
	}

	onEnemyState.ExecuteIfBound(this, _NewState);
}

void AIGC_Enemy::OnDelay_ChangeDisable()
{
	GetStatComp()->SetCharacterState(E_CHARACTER_STATE::DISABLE);
	if (disableHandle.IsValid())
		disableHandle.Invalidate();
}

void AIGC_Enemy::SetTarget()
{
	//현재 위치에서 제일 가까운 플레이어에게 간다
	FVector currentLoc{ GetActorLocation() };
	float nearestDistance{ std::numeric_limits<float>::max() };
	AIGC_Player* nearestPawn{};
	UWorld* world{ GetWorld() };
	for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		if (AIGC_Player* pawn = pIter->Get()->GetPawn<AIGC_Player>())
		{
			float curDistance{ static_cast<float>((pawn->GetActorLocation() - currentLoc).SquaredLength()) };
			if (curDistance >= nearestDistance)continue;

			nearestDistance = curDistance;
			nearestPawn = pawn;
		}
	}

	targetPlayer = nearestPawn ? nearestPawn : world->GetFirstPlayerController()->GetPawn<AIGC_Player>();
}

void AIGC_Enemy::RemoveTarget()
{
	targetPlayer = nullptr;
}

void AIGC_Enemy::OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	if (auto player = Cast<AIGC_Player>(_OtherActor))
	{
		overlappedPlayer.Emplace(player);
		if (damageHandle.IsValid()) return;
		damageHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AIGC_Enemy::ApplyDamage), damageInterval);
	}
}

void AIGC_Enemy::OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex)
{
	if (auto player = Cast<AIGC_Player>(_OtherActor))
	{
		if (!overlappedPlayer.Contains(player)) return;
		overlappedPlayer.Remove(player);

		if (overlappedPlayer.IsEmpty() && damageHandle.IsValid())
		{
			FTSTicker::GetCoreTicker().RemoveTicker(damageHandle);
			damageHandle.Reset();
		}
	}
}
