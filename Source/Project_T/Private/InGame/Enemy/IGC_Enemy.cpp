#include "InGame/Enemy/IGC_Enemy.h"
#include "InGame/IG_StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/Player/IGC_Player.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include <limits>

AIGC_Enemy::AIGC_Enemy(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	GetMesh()->bHiddenInGame = true;
	GetStatusWidget()->SetVisibility(false);

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

	UCapsuleComponent* enemyCollision{ GetCapsuleComponent() };

	GetStatComp()->onChangeState.AddUObject(this, &AIGC_Enemy::OnChangeState);
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
	GetStatComp()->SetCharacterState(E_CHARACTER_STATE::DISABLE);
}

void AIGC_Enemy::SetActive(bool _bIsActive)
{
	//적을 활성화 시킨다
	SetReplicateMovement(_bIsActive);
	SetReplicates(_bIsActive);
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

void AIGC_Enemy::OnChangeState(E_CHARACTER_STATE _NewState)
{
	switch (_NewState)
	{
	case E_CHARACTER_STATE::DISABLE: {
		SetActive(false);
		break;
	}
	case E_CHARACTER_STATE::ENABLE: {
		// 적의 상태를 활성화 시킨다
		GetStatusWidget()->SetVisibility(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MoveToNearestPlayer();
		break;
	}
	case E_CHARACTER_STATE::DEAD: {
		// 적이 죽었을 경우 추격을 멈추고 콜리전을 비활성화 한다
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController<AAIController>(), this);
		GetStatusWidget()->SetVisibility(false);
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

void AIGC_Enemy::MoveToNearestPlayer()
{
	//현재 위치에서 제일 가까운 플레이어에게 간다
	FVector currentLoc{ GetActorLocation() };
	float nearestDistance{ std::numeric_limits<float>::max() };
	APawn* nearestPawn{};
	UWorld* world{ GetWorld() };
	for (auto pIter = world->GetPlayerControllerIterator(); pIter; ++pIter)
	{
		if (APawn* pawn = pIter->Get()->GetPawn())
		{
			float curDistance{ static_cast<float>((pawn->GetActorLocation() - currentLoc).SquaredLength()) };
			if (curDistance >= nearestDistance)continue;

			nearestDistance = curDistance;
			nearestPawn = pawn;
		}
	}
	
	if (!nearestPawn)
		nearestPawn = world->GetFirstPlayerController()->GetPawn();

	PTT_LOG(Warning, TEXT("%s -> %s"), *GetName(), *nearestPawn->GetName());
	if (auto ai = GetController<AAIController>())
		ai->MoveToActor(nearestPawn);
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

		MoveToNearestPlayer();
	}
}
