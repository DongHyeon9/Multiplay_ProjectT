#pragma once

#include "InGame/Skill/IGS_ActorSapwnBase.h"
#include "IGS_Projectile.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECT_T_API UIGS_Projectile : public UIGS_ActorSapwnBase
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Projectile", meta = (AllowPrivateAccess = true))
	TArray<float> speeds{};

public:
	UIGS_Projectile(const FObjectInitializer& _Init);
	FName GetSkillName()const override { return TEXT("Projectile"); }

protected:
	void Excute()override;

};
