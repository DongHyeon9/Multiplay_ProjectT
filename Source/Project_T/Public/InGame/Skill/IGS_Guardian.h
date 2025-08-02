#pragma once

#include "InGame/Skill/IGS_ActorSapwnBase.h"
#include "IGS_Guardian.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECT_T_API UIGS_Guardian : public UIGS_ActorSapwnBase
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Guardian", meta = (AllowPrivateAccess = true))
	TArray<float> speeds{};

public:
	UIGS_Guardian(const FObjectInitializer& _Init);
	FName GetSkillName()const override { return TEXT("Guardian"); }

protected:
	void Excute()override;
};