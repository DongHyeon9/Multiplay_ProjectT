#include "InGame/Skill/IGS_Projectile.h"
#include "InGame/Skill/SkillActor/IGSA_Projectile.h"

namespace INGAME_PROJECTILE_SKILL
{
	static constexpr const TCHAR* SKILL_ACTOR_PATH{ TEXT("/Game/01_Blueprint/InGame/SKill/SkillActor/BP_IGSA_Projectile") };
}

UIGS_Projectile::UIGS_Projectile(const FObjectInitializer& _Init)
	:Super(_Init)
{
	static ConstructorHelpers::FClassFinder<AIGSA_Projectile> BP_PROJECTILE_SKILL_ACTOR{ INGAME_PROJECTILE_SKILL::SKILL_ACTOR_PATH };
	if (BP_PROJECTILE_SKILL_ACTOR.Succeeded())
		skillActorClass = BP_PROJECTILE_SKILL_ACTOR.Class;
}

void UIGS_Projectile::Excute()
{
	SERVER_ONLY();
	
	auto world{ skillInfo.owner->GetWorld() };
	PTT_ERRER(world);

	auto skillActor{ GetSkillActor<AIGSA_Projectile>() };
	PTT_ERRER(skillActor);

	const int32 currentSkillLevel{GetSkillLevel()};
	
	skillActor->SetActorLocation(skillInfo.owner->GetActorLocation());
	skillActor->SetActorScale3D(FVector{0.5f+currentSkillLevel*0.5f});
	FSkillActorInfo info{};
	info.skillLevel = currentSkillLevel;
	info.damage = GetSkillData()->skillDamages[currentSkillLevel - 1];
	skillActor->SetSkillActorInfo(info);
	skillActor->InitProjectile(speeds[currentSkillLevel - 1]);
	skillActor->ActiveSkill(durations[currentSkillLevel-1]);
}
