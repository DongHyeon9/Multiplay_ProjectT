#include "InGame/Skill/SkillActor/IGSA_Area.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/SphereComponent.h"
#include "InGame/Component/IG_StatComponent.h"

const FName AIGSA_Area::ROOT_COMP_NAME{ TEXT("RootComp") };
const FName AIGSA_Area::FX_COMP_NAME{ TEXT("FXComp") };
const FName AIGSA_Area::COLLISION_COMP_NAME{ TEXT("CollisionComp") };

namespace INGAME_AREA_SKILL_ACTOR
{
	static constexpr const TCHAR* FX_PATH{ TEXT("/Game/08_VFX/NS_RayStart") };
}

AIGSA_Area::AIGSA_Area(const FObjectInitializer& _Init)
	:Super(_Init)
{
	rootComp = CreateDefaultSubobject<USceneComponent>(ROOT_COMP_NAME);
	fxComp = CreateDefaultSubobject<UNiagaraComponent>(FX_COMP_NAME);
	collisionComp = CreateDefaultSubobject<USphereComponent>(COLLISION_COMP_NAME);

	RootComponent = rootComp;
	fxComp->SetupAttachment(rootComp);

	collisionComp->SetupAttachment(rootComp);
	collisionComp->ComponentTags.Emplace(IG_COLLISION_TAG::ATTACK);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FX_AREA{ INGAME_AREA_SKILL_ACTOR::FX_PATH };
	if (FX_AREA.Succeeded())
		fxComp->SetAsset(FX_AREA.Object);
		
}

void AIGSA_Area::InitSkillActor(const FSkillInitializeParameter& _Param)
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

	InactiveSkill();
}

void AIGSA_Area::BeginPlay()
{
	Super::BeginPlay();

	if (collisionComp)
	{
		if (!collisionComp->OnComponentBeginOverlap.IsAlreadyBound(this, &AIGSA_Area::OnBeginOverlap))
			collisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIGSA_Area::OnBeginOverlap);

		if (!collisionComp->OnComponentEndOverlap.IsAlreadyBound(this, &AIGSA_Area::OnEndOverlap))
			collisionComp->OnComponentEndOverlap.AddDynamic(this, &AIGSA_Area::OnEndOverlap);
	}

#if WITH_EDITOR
	collisionComp->SetHiddenInGame(false);
#endif
}

void AIGSA_Area::ActiveSkill(float _Duration)
{
	SERVER_ONLY();
	Super::ActiveSkill(_Duration);

	// 콜리전 활성화
	collisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

#if WITH_EDITOR
	OnRep_UpdateActive();
#endif
}

void AIGSA_Area::InactiveSkill()
{
	SERVER_ONLY();
	Super::InactiveSkill();

	// 콜리전 비활성화
	collisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	overlappedStatComp.Empty();

#if WITH_EDITOR
	OnRep_UpdateActive();
#endif
}

void AIGSA_Area::InitArea(float _Interval)
{
	interval = _Interval;
	acc = 0.0f;
}

void AIGSA_Area::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	// 데미지 적용 로직
	SERVER_ONLY();

	if (!IsActive()) return;

	acc += _DeltaTime;
	if (acc < interval) return;
	acc = 0.0f;
	const float damage{ skillInfo.ownerStat->GetDamage() + skillActorInfo.damage };

	// 물리 로직이 멀티 스레드에서 돌고있어 그냥 overlappedStatComp를 쓸 시
	// ensureMsgf(this->Array.Num() == InitialNum, TEXT("Container has changed during ranged-for iteration!"));
	// 에서 어설션이 걸림
	const auto damageCompList{ overlappedStatComp.Array() };
	for (auto statComp : damageCompList)
	{
		statComp->ApplyDamage(skillInfo.owner->GetController(), skillInfo.owner, damage);
	}
}

void AIGSA_Area::OnRep_UpdateActive()
{
	Super::OnRep_UpdateActive();

	// 이펙트 활성화
	if (IsActive())
	{
		fxComp->SetVisibility(true);
		fxComp->ResetSystem();
	}
	else
	{
		fxComp->SetVisibility(false);
	}
}

void AIGSA_Area::OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	if (!_OtherComp->ComponentHasTag(IG_COLLISION_TAG::CHARACTER)) return;
	if (auto statComp = _OtherActor->FindComponentByClass<UIG_StatComponent>())
		overlappedStatComp.Emplace(statComp);
}

void AIGSA_Area::OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex)
{
	if (auto statComp = _OtherActor->FindComponentByClass<UIG_StatComponent>())
		if (overlappedStatComp.Contains(statComp))
			overlappedStatComp.Remove(statComp);
}
