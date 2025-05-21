#include "InGame/Enemy/IGC_Enemy.h"
#include "InGame/IG_StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

AIGC_Enemy::AIGC_Enemy(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_01(TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_01"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_ENEMY_02(TEXT("/Game/03_Material/Instances/Manny/MI_Enemy_02"));

	auto mesh = GetMesh();
	if (MI_ENEMY_01.Succeeded()) mesh->SetMaterial(0, MI_ENEMY_01.Object);
	if (MI_ENEMY_02.Succeeded()) mesh->SetMaterial(1, MI_ENEMY_02.Object);
}

void AIGC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	GetStatComp()->onChangeState.AddUObject(this, &AIGC_Enemy::OnChangeState);
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

void AIGC_Enemy::OnChangeState(E_CHARACTER_STATE _NewState)
{
	switch (_NewState)
	{
	case E_CHARACTER_STATE::DISABLE: {
		SetActive(false);
		break;
	}
	case E_CHARACTER_STATE::ENABLE: {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
	case E_CHARACTER_STATE::DEAD: {
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