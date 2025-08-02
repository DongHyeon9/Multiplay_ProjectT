#include "InGame/Skill/IGS_Area.h"
#include "InGame/Skill/SkillActor/IGSA_Area.h"

namespace INGAME_AREA_SKILL
{
	static constexpr const TCHAR* SKILL_ACTOR_PATH{ TEXT("/Game/01_Blueprint/InGame/SKill/SkillActor/BP_IGSA_Area") };
}

UIGS_Area::UIGS_Area(const FObjectInitializer& _Init)
	:Super(_Init)
{
	static ConstructorHelpers::FClassFinder<AIGSA_Area> BP_AREA_SKILL_ACTOR{ INGAME_AREA_SKILL::SKILL_ACTOR_PATH };
	if (BP_AREA_SKILL_ACTOR.Succeeded())
		skillActorClass = BP_AREA_SKILL_ACTOR.Class;
}

void UIGS_Area::Excute()
{
	SERVER_ONLY();
	
	auto world{ skillInfo.owner->GetWorld() };
	PTT_ERRER(world);

	auto skillActor{ GetSkillActor<AIGSA_Area>() };
	PTT_ERRER(skillActor);

	const int32 currentSkillLevel{ GetSkillLevel() };

	skillActor->SetActorLocationAndRotation(skillInfo.owner->GetActorLocation(), FRotator{});
	FSkillActorInfo info{};
	info.skillLevel = currentSkillLevel;
	info.damage = GetSkillData()->skillDamages[currentSkillLevel - 1];
	skillActor->SetSkillActorInfo(info);
	skillActor->InitArea(intervals[currentSkillLevel - 1]);
	skillActor->ActiveSkill(durations[currentSkillLevel - 1]);
}
