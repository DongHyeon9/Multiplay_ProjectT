#include "InGame/Skill/IG_SkillBase.h"
#include "InGame/Component/Player/IG_SkillComponent.h"
#include "InGame/Component/IG_StatComponent.h"
#include "Net/UnrealNetwork.h"

namespace INGAME_SKILL_BASE
{
    static constexpr const TCHAR* SKILL_DATA_TABLE_PATH{ TEXT("/Game/07_GameData/CSV/CV_SKillData") };
}

UIG_SkillBase::UIG_SkillBase(const FObjectInitializer& _Initializer):Super(_Initializer)
{
    static ConstructorHelpers::FObjectFinder<UDataTable> CV_SKILL_DATA{ INGAME_SKILL_BASE::SKILL_DATA_TABLE_PATH };
    if (CV_SKILL_DATA.Succeeded()) skillDataTable = CV_SKILL_DATA.Object;
}

void UIG_SkillBase::InitSkill(const FSkillInitializeParameter& _Initializer)
{
    timerDelegate.BindUObject(this, &UIG_SkillBase::Excute_Internal);
    skillInfo = _Initializer;
    skillData = skillDataTable->FindRow<FSkillData>(GetSkillName(), TEXT("Skill Data Load"));
    PTT_LOG(Warning, TEXT("InitSkill : %s"), *GetSkillName().ToString());
    SetSkillLevel(SKILL_MIN_LEVEL);
}

void UIG_SkillBase::Excute_Internal()
{
    Excute();
}

void UIG_SkillBase::UnRegistSkill()
{
    SERVER_ONLY();
    
    if (!skillInfo.owner->HasAuthority()) return;
    if (!coolDownHandle.IsValid()) return;
    GetWorld()->GetTimerManager().ClearTimer(coolDownHandle);
}

void UIG_SkillBase::Excute()
{
    SERVER_ONLY();
    Multicast_Excute();
}

void UIG_SkillBase::SetSkillLevel(int32 _NewLevel)
{
    SERVER_ONLY();

    skillLevel = _NewLevel;
    UnRegistSkill();
    GetWorld()->GetTimerManager().SetTimer(coolDownHandle, timerDelegate, skillData->skillCoolDown[skillLevel - 1], true);
}

void UIG_SkillBase::DestroySkill()
{
    UnRegistSkill();
}

void UIG_SkillBase::LevelUp()
{
    SetSkillLevel(FMath::Clamp(skillLevel + 1, SKILL_MIN_LEVEL, SKILL_MAX_LEVEL));
}

void UIG_SkillBase::ApplyDamage(AActor* _Target)
{
    SERVER_ONLY();
    
    const int32 currentSkillLevel{ GetSkillLevel() };

    auto statComp{ _Target->FindComponentByClass<UIG_StatComponent>() };
    if (!statComp) return;

    auto currentSkillData{ GetSkillData() };
    PTT_ERRER(currentSkillData->skillDamages.IsValidIndex(currentSkillLevel - 1));
    statComp->ApplyDamage(
        skillInfo.owner->GetController(),
        _Target,
        currentSkillData->skillDamages[currentSkillLevel - 1] + skillInfo.ownerStat->GetDamage());
}

bool UIG_SkillBase::HasAuthority()
{
    return skillInfo.owner->HasAuthority(); 
}

UWorld* UIG_SkillBase::GetWorld() const
{
    check(skillInfo.owner);
    return skillInfo.owner->GetWorld();
}

void UIG_SkillBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UIG_SkillBase, skillLevel);
    DOREPLIFETIME(UIG_SkillBase, skillInfo);
}

int32 UIG_SkillBase::GetFunctionCallspace(UFunction* _Function, FFrame* _Stack)
{
    return skillInfo.ownerComp->GetFunctionCallspace(_Function, _Stack);
}

bool UIG_SkillBase::CallRemoteFunction(UFunction* _Function, void* _Parms, FOutParmRec* _OutParms, FFrame* _Stack)
{
    if (skillInfo.owner)
    {
        if (UNetDriver* netDriver = skillInfo.owner->GetNetDriver())
        {
            netDriver->ProcessRemoteFunction(skillInfo.owner, _Function, _Parms, _OutParms, _Stack, this);
            return true;
        }
    }
    return false;
}