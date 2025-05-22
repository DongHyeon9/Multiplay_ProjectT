#include "InGame/Enemy/IGC_Enemy.h"
#include "InGame/IG_StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/Enemy/IG_AIController.h"
#include "Components/WidgetComponent.h"

AIGC_Enemy::AIGC_Enemy(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_01(TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_01"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_02(TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_02"));
	static ConstructorHelpers::FClassFinder<AAIController> AI_CONTROLLER(TEXT("/Game/01_Blueprint/InGame/Enemy/BP_IG_AIController"));

	auto mesh = GetMesh();
	if (MI_ENEMY_01.Succeeded()) mesh->SetMaterial(0, MI_ENEMY_01.Object);
	if (MI_ENEMY_02.Succeeded()) mesh->SetMaterial(1, MI_ENEMY_02.Object);
	if (AI_CONTROLLER.Succeeded()) AIControllerClass = AI_CONTROLLER.Class;
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
	bReplicates = false;
	GetMesh()->bHiddenInGame = true;
	GetStatComp()->SetCharacterState(E_CHARACTER_STATE::DISABLE);
}

void AIGC_Enemy::SetActive(bool _bIsActive)
{
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
		GetStatusWidget()->SetHiddenInGame(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
	case E_CHARACTER_STATE::DEAD: {
		GetStatusWidget()->SetHiddenInGame(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (disableHandle.IsValid())
			disableHandle.Invalidate();
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
