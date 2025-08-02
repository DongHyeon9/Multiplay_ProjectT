#include "InGame/Skill/SkillActor/IG_SkillActorBase.h"
#include "Net/UnrealNetwork.h"

AIG_SkillActorBase::AIG_SkillActorBase(const FObjectInitializer& _Init)
	:Super(_Init)
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicateMovement(true);
	SetReplicates(true);
}

void AIG_SkillActorBase::InitSkillActor(const FSkillInitializeParameter& _Param)
{
	SERVER_ONLY();
	skillInfo = _Param;
	InactiveSkill();
}

void AIG_SkillActorBase::ActiveSkill(float _Duration)
{
	SERVER_ONLY();
	bIsActive=true;
	SetNetDormancy(ENetDormancy::DORM_Awake);
	SetActorTickEnabled(bIsActive);

	if (FMath::IsNearlyZero(_Duration)) return;

	auto world{GetWorld()};
	if (!world) return;
	world->GetTimerManager().SetTimer(activeHandle,FTimerDelegate::CreateUObject(this,&AIG_SkillActorBase::InactiveSkill_Internal),_Duration,false);
}

void AIG_SkillActorBase::InactiveSkill()
{
	SERVER_ONLY();
	bIsActive=false;
	SetNetDormancy(ENetDormancy::DORM_DormantAll);
	SetActorTickEnabled(bIsActive);
}

void AIG_SkillActorBase::SetSkillActorInfo(const FSkillActorInfo& _SkillActorInfo)
{
	skillActorInfo=_SkillActorInfo;
}

void AIG_SkillActorBase::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void AIG_SkillActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIG_SkillActorBase, skillInfo);
	DOREPLIFETIME(AIG_SkillActorBase, bIsActive);
	DOREPLIFETIME(AIG_SkillActorBase, skillActorInfo);
}

void AIG_SkillActorBase::InactiveSkill_Internal()
{
	InactiveSkill();
}
