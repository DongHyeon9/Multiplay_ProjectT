#pragma once

#include "../Project_T.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "Containers/Ticker.h"
#include "IG_SkillBase.generated.h"

class UAnimMontage;

UCLASS(Abstract)
class PROJECT_T_API AIG_SkillBase : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_SkillBase")
	TObjectPtr<UAnimMontage> animMontage{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_SkillBase")
	FName skillName{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_SkillBase")
	uint8 level{0};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_SkillBase")
	float damage{ 20.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_SkillBase")
	uint8 duration{ 1 };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_SkillBase")
	float coolDown{3.f};

	FDamageEvent DamageEvent;
private:
	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly, category = "AIG_SkillBase", meta = (AllowPrivateAccess = true))
	bool bUseSkill{};

	FTSTicker::FDelegateHandle coolDownHandle{};
	
public:
	virtual void BeginPlay();
	virtual void InitSkill();
	virtual void UseSkill() {}
	//서버에서 호출하면 모든 클라이언트에서 스킬사용을 종료함 -> 게임 종료시 호출해야됨
	void SetSkillUse(bool _bIsUse);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ActivateSkill(UStaticMeshComponent* _Mesh);
	virtual void DisableSkill(UStaticMeshComponent* _Mesh);

private:
	bool SkillInterval(float _DeltaTime);

};
