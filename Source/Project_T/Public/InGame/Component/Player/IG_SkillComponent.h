#pragma once

#include "../Project_T.h"
#include "Components/ActorComponent.h"
#include "IG_SkillComponent.generated.h"

class UIG_SkillBase;

UCLASS(ClassGroup = (Project_T), meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIG_SkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:

protected:

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIG_SkillComponent", meta = (AllowPrivateAccess = true))
	TArray<TEnumAsByte<EObjectTypeQuery>> collisionType{};

	UPROPERTY()
	UDataTable* skillInfoTable{};

#if WITH_EDITORONLY_DATA 
	TMap<FName, const FSkillInfo*> skillInfoEditorClient{};

	UPROPERTY()
	UDataTable* skillInfoTableEditorClient{};
#endif

	TMap<FName, const FSkillInfo*> skillInfoMap{};

	UPROPERTY(VisibleInstanceOnly)
	TMap<FName, UIG_SkillBase*> skills{};

	//스킬을 배울 수 있는 횟수
	UPROPERTY(VisibleInstanceOnly)
	int32 acquisitionCount{};

	TArray<FName> acquisitionSkillList{};

public:
	UIG_SkillComponent(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void EndPlay(EEndPlayReason::Type _Reason);

	void AddAcquisitionCount(int32 _Count);
	void RegistSkill(FName _SkillName);
	void DestroySkills();

	void GetRandomSkills(TArray<FName>& _OutResult);

	int32 GetSkillLevel(FName _SkillName)const;
	const FSkillInfo_Server* GetSkillInfo_Server(FName _Key) const;
	const FSkillInfo_Client* GetSkillInfo_Client(FName _Key) const;
	FORCEINLINE const TMap<FName, const FSkillInfo*>& GetSkillInfoMap() const { return skillInfoMap; }
	FORCEINLINE bool IsValidSkill(FName _SkillName) { return acquisitionSkillList.Contains(_SkillName); }
	FORCEINLINE int32 GetAcquisitionCount()const { return acquisitionCount; }

protected:

private:

};