#include "InGame/Skill/SkillActor/IGSA_Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InGame/Component/IG_StatComponent.h"

namespace INGAME_PROJECTILE_SKILL_ACTOR
{
	static constexpr const TCHAR* MESH_PATH{ TEXT("/Game/08_VFX/SM_Meteor") };
}

const FName AIGSA_Projectile::ROOT_COMP_NAME{ TEXT("RootComp") };
const FName AIGSA_Projectile::MESH_COMP_NAME{ TEXT("MeshComp") };
const FName AIGSA_Projectile::COLLISION_COMP_NAME{ TEXT("CollisionComp") };

AIGSA_Projectile::AIGSA_Projectile(const FObjectInitializer& _Init)
	:Super(_Init)
{
	// 컴포넌트 초기화
	rootComp = CreateDefaultSubobject<USceneComponent>(ROOT_COMP_NAME);
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(MESH_COMP_NAME);
	collisionComp = CreateDefaultSubobject<USphereComponent>(COLLISION_COMP_NAME);

	RootComponent = rootComp;

	meshComp->SetupAttachment(rootComp);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	collisionComp->SetupAttachment(rootComp);
	collisionComp->ComponentTags.Emplace(IG_COLLISION_TAG::ATTACK);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_METEOR{ INGAME_PROJECTILE_SKILL_ACTOR::MESH_PATH };
	if (SM_METEOR.Succeeded())
		meshComp->SetStaticMesh(SM_METEOR.Object);
}

void AIGSA_Projectile::InitSkillActor(const FSkillInitializeParameter& _Param)
{
	Super::InitSkillActor(_Param);

	// 콜리전 초기화
	collisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	collisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	collisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	for (auto& objectTypeQuery : _Param.collisionType)
	{
		ECollisionChannel channel = UEngineTypes::ConvertToCollisionChannel(objectTypeQuery);
		collisionComp->SetCollisionResponseToChannel(channel, ECollisionResponse::ECR_Overlap);
	}
}

void AIGSA_Projectile::BeginPlay()
{
	Super::BeginPlay();

	// 델리게이트 연결
	if (HasAuthority())
	{
		if (!collisionComp->OnComponentBeginOverlap.IsAlreadyBound(this, &AIGSA_Projectile::OnBeginOverlap))
			collisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIGSA_Projectile::OnBeginOverlap);
	}

#if WITH_EDITOR
	collisionComp->SetHiddenInGame(false);
#endif
}

void AIGSA_Projectile::InitProjectile(float _Speed)
{
	speed = _Speed;
	direction = skillInfo.owner->GetActorForwardVector();
}

void AIGSA_Projectile::ActiveSkill(float _Duration)
{
	SERVER_ONLY();
	Super::ActiveSkill(_Duration);

	// 콜리전 활성화
	collisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
#if WITH_EDITOR
	OnRep_UpdateActive();
#endif
}

void AIGSA_Projectile::InactiveSkill()
{
	SERVER_ONLY();
	Super::InactiveSkill();

	// 콜리전 비활성화
	collisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
#if WITH_EDITOR
	OnRep_UpdateActive();
#endif
}

void AIGSA_Projectile::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	// 이동
	SetActorLocation(GetActorLocation() + direction * speed, true);
}

void AIGSA_Projectile::OnRep_UpdateActive()
{
	Super::OnRep_UpdateActive();

	// 메쉬 비지블
	meshComp->SetVisibility(IsActive());
}

void AIGSA_Projectile::OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	// 데미지 로직
	if (!_OtherComp->ComponentHasTag(IG_COLLISION_TAG::CHARACTER)) return;

	auto statComp{ _OtherActor->FindComponentByClass<UIG_StatComponent>() };
	if (!statComp) return;
	statComp->ApplyDamage(skillInfo.owner->GetController(), skillInfo.owner, skillActorInfo.damage + skillInfo.ownerStat->GetDamage());

	// 비활성화
	InactiveSkill();
}
