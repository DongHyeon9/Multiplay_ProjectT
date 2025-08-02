#include "InGame/Skill/IGS_ActorSapwnBase.h"
#include "InGame/Skill/SkillActor/IG_SkillActorBase.h"
#include "Net/UnrealNetwork.h"

void UIGS_ActorSapwnBase::SetSkillLevel(int32 _NewLevel)
{
	SERVER_ONLY();
	
	Super::SetSkillLevel(_NewLevel);

	const int32 currentSkillLevel{GetSkillLevel()};
	auto currentSkillData{ GetSkillData() };
	FSkillActorInfo actorInfo{};
	actorInfo.skillLevel=currentSkillLevel;
	actorInfo.damage=currentSkillData->skillDamages[currentSkillLevel - 1];
	for (auto skillActor : skillActorPool)
	{
		skillActor->SetSkillActorInfo(actorInfo);
	}
}

void UIGS_ActorSapwnBase::InitSkill(const FSkillInitializeParameter& _Initializer)
{
	Super::InitSkill(_Initializer);

	ExpandPool();
}

void UIGS_ActorSapwnBase::DestroySkill()
{
	Super::DestroySkill();

	for (auto skillActor : skillActorPool)
	{
		if (skillActor->IsActive())
			skillActor->ActiveSkill(false);
		skillActor->Destroy();
	}
	skillActorPool.Empty();
}

void UIGS_ActorSapwnBase::ExpandPool()
{
	SERVER_ONLY();
	
	UWorld* world{ GetWorld() };

	poolSize = FMath::Abs(poolSize);
	poolSize *= 2;
	++poolSize;
	
	skillActorPool.Reserve(poolSize);

	while (skillActorPool.Num() < poolSize)
	{
		auto skillActor{ world->SpawnActorDeferred<AIG_SkillActorBase>(skillActorClass, FTransform{},skillInfo.owner->GetInstigatorController(),skillInfo.owner) };
		skillActor->InitSkillActor(skillInfo);
		skillActor->FinishSpawning(FTransform{});
		skillActorPool.Emplace(skillActor);
	}
}
