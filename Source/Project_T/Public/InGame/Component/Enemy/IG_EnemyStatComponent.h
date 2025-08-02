#pragma once

#include "InGame/Component/IG_StatComponent.h"
#include "IG_EnemyStatComponent.generated.h"

UCLASS(ClassGroup = (Project_T), meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIG_EnemyStatComponent : public UIG_StatComponent
{
	GENERATED_BODY()
public:

protected:

private:
	int32 tableMax{};

public:
	UIG_EnemyStatComponent(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void InitStat(int32 _Idx)override;
	void ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage) override;

	FORCEINLINE int32 GetTableMax()const { return tableMax; }

protected:
	
private:

};