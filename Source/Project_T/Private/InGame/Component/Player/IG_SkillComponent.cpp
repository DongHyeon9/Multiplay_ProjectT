#include "InGame/Component/Player/IG_SkillComponent.h"
#include "InGame/Skill/IG_SkillBase.h"
#include "InGame/Component/IG_StatComponent.h"

namespace INGAME_SKILL_COMPONENT
{
	static constexpr const TCHAR* SKILL_INFO_SERVER_TABLE_PATH{ TEXT("/Game/07_GameData/CSV/CV_SKillInfo_Server") };
	static constexpr const TCHAR* SKILL_INFO_CLIENT_TABLE_PATH{ TEXT("/Game/07_GameData/CSV/CV_SKillInfo_Client") };
}

UIG_SkillComponent::UIG_SkillComponent(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UIG_SkillComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		skillInfoTable = LoadObject<UDataTable>(this, INGAME_SKILL_COMPONENT::SKILL_INFO_SERVER_TABLE_PATH);
#if WITH_EDITOR
		skillInfoTableEditorClient = LoadObject<UDataTable>(this, INGAME_SKILL_COMPONENT::SKILL_INFO_CLIENT_TABLE_PATH);
#endif
	}
	else
	{
		skillInfoTable = LoadObject<UDataTable>(this, INGAME_SKILL_COMPONENT::SKILL_INFO_CLIENT_TABLE_PATH);
	}

	TArray<FSkillInfo*> skillInfoList{};
	skillInfoTable->GetAllRows<FSkillInfo>(TEXT("Skill Info Load"), skillInfoList);
	for (auto skillInfo : skillInfoList)
	{
		skillInfoMap.Emplace(skillInfo->skillName, skillInfo);
	}
#if WITH_EDITOR
	skillInfoList.Empty();
	skillInfoTableEditorClient->GetAllRows<FSkillInfo>(TEXT("Skill Info Load"), skillInfoList);
	for (auto skillInfo : skillInfoList)
	{
		skillInfoEditorClient.Emplace(skillInfo->skillName, skillInfo);
	}
#endif
}

void UIG_SkillComponent::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	DestroySkills();
}

void UIG_SkillComponent::AddAcquisitionCount(int32 _Count)
{
	acquisitionCount += _Count;
}

void UIG_SkillComponent::RegistSkill(FName _SkillName)
{
	SERVER_ONLY_COMP();
	PTT_ERRER(acquisitionCount > 0);
	--acquisitionCount;

	if (skills.Contains(_SkillName))
		skills[_SkillName]->LevelUp();
	else
	{
		auto skillInfo{ GetSkillInfo_Server(_SkillName) };
		PTT_ERRER(skillInfo);
		auto skill{ NewObject<UIG_SkillBase>(GetOwner(), skillInfo->skillClass) };
		AddReplicatedSubObject(skill);
		PTT_ERRER(skill);
		auto owner{ GetOwner<APawn>() };
		PTT_ERRER(owner);
		auto ownerStat{ owner->FindComponentByClass<UIG_StatComponent>() };
		PTT_ERRER(ownerStat);

		FSkillInitializeParameter param{};
		param.collisionType = collisionType;
		param.owner = owner;
		param.ownerComp = this;
		param.ownerStat = ownerStat;
		skill->InitSkill(param);

		skills.Emplace(_SkillName, skill);
	}
}

void UIG_SkillComponent::DestroySkills()
{
	for (auto& skill : skills)
	{
		skill.Value->DestroySkill();
	}
}

void UIG_SkillComponent::GetRandomSkills(TArray<FName>& _OutResult)
{
	_OutResult.Empty();
	acquisitionSkillList.Empty();
	if (skills.Num() >= SKILL_MAX_REGIST_COUNT)
	{
		skills.GenerateKeyArray(_OutResult);
		// 다 배운 스킬 예외처리
		_OutResult.RemoveAll([this](const FName& _SkillName) { return skills[_SkillName]->GetSkillLevel() >= SKILL_MAX_LEVEL; });
	}
	else
	{
		// Fisher–Yates 알고리즘
		TArray<FName> keys{};
		skillInfoMap.GenerateKeyArray(keys);
		// 다 배운 스킬 예외처리
		keys.RemoveAll([this](const FName& _SkillName) { 
			if (skills.Contains(_SkillName))
				return skills[_SkillName]->GetSkillLevel() >= SKILL_MAX_LEVEL;
			return false;
			});

		const int32 lastIdx{ keys.Num() - 1 };
		for (int32 i = 0; i < keys.Num(); ++i)
		{
			int32 j{ FMath::RandRange(i,lastIdx) };
			keys.Swap(i, j);
		}

		_OutResult.Append(keys.GetData(), FMath::Min(SKILL_MAX_REGIST_COUNT, keys.Num()));
	}

	acquisitionSkillList = _OutResult;
}

int32 UIG_SkillComponent::GetSkillLevel(FName _SkillName) const
{
	if (skills.Contains(_SkillName))
		return skills[_SkillName]->GetSkillLevel();
	return 0;
}

const FSkillInfo_Server* UIG_SkillComponent::GetSkillInfo_Server(FName _Key) const
{
	if (skillInfoMap.Contains(_Key))
		return reinterpret_cast<const FSkillInfo_Server*>(skillInfoMap[_Key]);
	return nullptr;
}

const FSkillInfo_Client* UIG_SkillComponent::GetSkillInfo_Client(FName _Key) const
{
#if WITH_EDITOR
	if (skillInfoEditorClient.Contains(_Key))
		return reinterpret_cast<const FSkillInfo_Client*>(skillInfoEditorClient[_Key]);
	return nullptr;
#endif
#if !WITH_EDITOR
	if (skillInfoMap.Contains(_Key))
		return reinterpret_cast<const FSkillInfo_Client*>(skillInfoMap[_Key]);
	return nullptr;
#endif
}
