#pragma once

#include "../Project_T.h"
#include "InGame/IG_CharacterBase.h"
#include "IGC_Enemy.generated.h"

enum class E_CHARACTER_STATE : uint8;

DECLARE_DELEGATE_TwoParams(FOnEnemyStateDelegate, AIGC_Enemy*, E_CHARACTER_STATE);

UCLASS()
class PROJECT_T_API AIGC_Enemy : public AIG_CharacterBase
{
	GENERATED_BODY()
public:
	FOnEnemyStateDelegate onEnemyState{};

private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,category="AIGC_Enemy",meta=(AllowPrivateAccess=true))
	float disableDelay{ 5.0f };

	FTimerHandle disableHandle{};

public:
	AIGC_Enemy(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void InitEnemy();
	void SetActive(bool _bIsActive);

private:
	void OnChangeState(E_CHARACTER_STATE _NewState);
	void OnDelay_ChangeDisable();
};
