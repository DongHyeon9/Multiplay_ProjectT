#include "InGame/Player/IG_SkillComponent.h"
#include "InGame/Skill/IG_SkillBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

void UIG_SkillComponent::RegistSkill(TSubclassOf<AIG_SkillBase> _SkillClass)
{
	FActorSpawnParameters param{};
	param.Owner = GetOwner();
	auto newSkill = GetWorld()->SpawnActor<AIG_SkillBase>(_SkillClass, param);
	newSkill->InitSkill();
	skillList.Emplace(newSkill);
}

void UIG_SkillComponent::EndGame()
{
	for (auto skill : skillList)
	{
		skill->SetSkillUse(false);
	}
}

void UIG_SkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIG_SkillComponent, skillList);
}
