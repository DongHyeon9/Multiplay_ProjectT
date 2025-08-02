#include "InGame/Skill/IGS_Guardian.h"
#include "InGame/Skill/SkillActor/IGSA_Guardian.h"

namespace INGAME_GUARDIAN_SKILL
{
	static constexpr const TCHAR* SKILL_ACTOR_PATH{ TEXT("/Game/01_Blueprint/InGame/SKill/SkillActor/BP_IGSA_Guardian") };
}

UIGS_Guardian::UIGS_Guardian(const FObjectInitializer& _Init)
	:Super(_Init)
{
	static ConstructorHelpers::FClassFinder<AIGSA_Guardian> BP_GUARDIAN_SKILL_ACTOR{ INGAME_GUARDIAN_SKILL::SKILL_ACTOR_PATH };
	if (BP_GUARDIAN_SKILL_ACTOR.Succeeded())
		skillActorClass = BP_GUARDIAN_SKILL_ACTOR.Class;
}

void UIGS_Guardian::Excute()
{
	SERVER_ONLY();
	
	auto world{ skillInfo.owner->GetWorld() };
	PTT_ERRER(world);

	auto skillActor{ GetSkillActor<AIGSA_Guardian>() };
	PTT_ERRER(skillActor);

	const int32 currentSkillLevel{ GetSkillLevel() };

	skillActor->InitGuardian(speeds[currentSkillLevel - 1]);
	FSkillActorInfo info{};
	info.skillLevel = currentSkillLevel;
	info.damage = GetSkillData()->skillDamages[currentSkillLevel - 1];
	skillActor->SetSkillActorInfo(info);
	skillActor->ActiveSkill(durations[currentSkillLevel - 1]);
}
