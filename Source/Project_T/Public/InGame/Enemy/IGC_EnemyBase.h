#pragma once

#include "../Project_T.h"
#include "InGame/IG_CharacterBase.h"
#include "IGC_EnemyBase.generated.h"

enum class E_CHARACTER_STATE : uint8;

DECLARE_DELEGATE_TwoParams(FOnEnemyStateDelegate, AIGC_EnemyBase*, E_CHARACTER_STATE);

UCLASS()
class PROJECT_T_API AIGC_EnemyBase : public AIG_CharacterBase
{
	GENERATED_BODY()
public:
	FOnEnemyStateDelegate onEnemyState{};

public:
	AIGC_EnemyBase(const FObjectInitializer& _Initializer);
	void BeginPlay()override;

private:
	void OnChangeState(E_CHARACTER_STATE _NewState);
};
