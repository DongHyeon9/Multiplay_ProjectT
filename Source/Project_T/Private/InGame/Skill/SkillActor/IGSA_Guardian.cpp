#include "InGame/Skill/SkillActor/IGSA_Guardian.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "InGame/Component/IG_StatComponent.h"
#include "Kismet/KismetMathLibrary.h"

namespace INGAME_GUARDIAN_SKILL_ACTOR
{
	static constexpr const TCHAR* MESH_PATH{ TEXT("/Game/02_Mesh/SM_Ball_01") };
}

const FName AIGSA_Guardian::ROOT_COMP_NAME{ TEXT("RootComp") };

AIGSA_Guardian::AIGSA_Guardian(const FObjectInitializer& _Init)
	:Super(_Init)
{
	// 컴포넌트 초기화
	rootComp = CreateDefaultSubobject<USceneComponent>(ROOT_COMP_NAME);
	RootComponent = rootComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BALL{ INGAME_GUARDIAN_SKILL_ACTOR::MESH_PATH };
	check(SM_BALL.Succeeded());

	for (int32 i = 0; i < SKILL_MAX_LEVEL; ++i)
	{
		TObjectPtr<UStaticMeshComponent> meshComp = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("MeshComp_%d"), i));
		TObjectPtr<USphereComponent> collisionComp = CreateDefaultSubobject<USphereComponent>(*FString::Printf(TEXT("CollisionComp_%d"), i));

		collisionComp->SetupAttachment(rootComp);
		collisionComp->ComponentTags.Emplace(IG_COLLISION_TAG::ATTACK);

#if WITH_EDITOR
		collisionComp->SetHiddenInGame(false);
#endif

		meshComp->SetupAttachment(collisionComp);
		meshComp->SetStaticMesh(SM_BALL.Object);
		meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		meshComp->SetRelativeScale3D(FVector{ 0.75f });

		meshComps.Emplace(meshComp);
		collisionComps.Emplace(collisionComp);
	}
}

void AIGSA_Guardian::InitSkillActor(const FSkillInitializeParameter& _Param)
{
	Super::InitSkillActor(_Param);

	// 콜리전 초기화
	for (auto collisionComp : collisionComps)
	{
		collisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		collisionComp->SetCollisionObjectType(ECC_WorldDynamic);
		collisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		for (auto& objectTypeQuery : _Param.collisionType)
		{
			ECollisionChannel channel = UEngineTypes::ConvertToCollisionChannel(objectTypeQuery);
			collisionComp->SetCollisionResponseToChannel(channel, ECollisionResponse::ECR_Overlap);
		}
		if (!collisionComp->OnComponentBeginOverlap.IsAlreadyBound(this, &AIGSA_Guardian::OnBeginOverlap))
			collisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIGSA_Guardian::OnBeginOverlap);
	}
}

void AIGSA_Guardian::ActiveSkill(float _Duration)
{
	SERVER_ONLY();
	Super::ActiveSkill(_Duration);

	const float angle{ static_cast<float>(UKismetMathLibrary::GetTAU()) / skillActorInfo.skillLevel };

	// 콜리전 활성화
	for (int32 i = 0; i < skillActorInfo.skillLevel; ++i)
	{
		const float currentAngle{ angle * i };
		const float xCoord{ static_cast<float>(collisionDistance.X * FMath::Cos(currentAngle) - collisionDistance.Y * FMath::Sin(currentAngle)) };
		const float yCoord{ static_cast<float>(collisionDistance.X * FMath::Sin(currentAngle) + collisionDistance.Y * FMath::Cos(currentAngle)) };
		collisionComps[i]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		collisionComps[i]->SetRelativeLocation(FVector{ xCoord, yCoord, 0.0f });
	}

#if WITH_EDITOR
	OnRep_UpdateActive();
#endif
}

void AIGSA_Guardian::InactiveSkill()
{
	SERVER_ONLY();
	Super::InactiveSkill();

	// 콜리전 비활성화
	for (auto collisionComp : collisionComps)
	{
		collisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

#if WITH_EDITOR
	OnRep_UpdateActive();
#endif
}

void AIGSA_Guardian::InitGuardian(float _Speed)
{
	SERVER_ONLY();

	speed = _Speed;
}

void AIGSA_Guardian::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (!IsActive()) return;
	// 이펙트 회전
	SetActorLocation(skillInfo.owner->GetActorLocation());
	rootComp->AddRelativeRotation(FRotator{ 0.0f, speed, 0.0f });
}

void AIGSA_Guardian::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIGSA_Guardian, speed);
}

void AIGSA_Guardian::OnRep_UpdateActive()
{
	Super::OnRep_UpdateActive();

	if (IsActive())
	{
		for (int32 i = 0; i < skillActorInfo.skillLevel; ++i)
		{
			meshComps[i]->SetVisibility(true);
		}
	}
	else
	{
		// 메시 비활성화
		for (auto meshComp : meshComps)
		{
			meshComp->SetVisibility(false);
		}
	}
}

void AIGSA_Guardian::OnConstruction(const FTransform& _Transform)
{
	Super::OnConstruction(_Transform);

	for (auto collisionComp : collisionComps)
	{
		collisionComp->SetSphereRadius(collisionRadius);
	}
}

void AIGSA_Guardian::OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	// 데미지 로직
	if (!_OtherComp->ComponentHasTag(IG_COLLISION_TAG::CHARACTER)) return;

	if (auto statComp{ _OtherActor->FindComponentByClass<UIG_StatComponent>() })
		statComp->ApplyDamage(skillInfo.owner->GetController(), skillInfo.owner, skillInfo.ownerStat->GetDamage() + skillActorInfo.damage);
}
