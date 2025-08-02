#pragma once

#include "InGame/Skill/IGS_ActorSapwnBase.h"
#include "IGS_Area.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECT_T_API UIGS_Area : public UIGS_ActorSapwnBase
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGS_Area", meta = (AllowPrivateAccess = true))
	TArray<float> intervals{};

public:
	UIGS_Area(const FObjectInitializer& _Init);
	FName GetSkillName()const override{ return TEXT("Area"); }

protected:
	void Excute()override;
};
