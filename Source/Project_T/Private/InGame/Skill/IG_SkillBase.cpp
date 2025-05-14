#include "InGame/Skill/IG_SkillBase.h"
#include "Net/UnrealNetwork.h"

AIG_SkillBase::AIG_SkillBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AIG_SkillBase::UseSkill()
{
}



void AIG_SkillBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Skill Used: %s"), *SkillInfo.Name.ToString());
}

void AIG_SkillBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIG_SkillBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AIG_SkillBase, SkillInfo);
}

