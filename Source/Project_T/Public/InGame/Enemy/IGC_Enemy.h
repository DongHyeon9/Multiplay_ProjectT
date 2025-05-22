#pragma once

#include "../Project_T.h"
#include "InGame/IG_CharacterBase.h"
#include "Containers/Ticker.h"
#include "IGC_Enemy.generated.h"

class AIGC_Player;

enum class E_CHARACTER_STATE : uint8;

DECLARE_DELEGATE_TwoParams(FOnEnemyStateDelegate, AIGC_Enemy*, E_CHARACTER_STATE);

UCLASS()
class PROJECT_T_API AIGC_Enemy : public AIG_CharacterBase
{
	GENERATED_BODY()
public:
	FOnEnemyStateDelegate onEnemyState{};

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGC_Enemy", meta = (AllowPrivateAccess = true))
	float disableDelay{ 5.0f };

	FTimerHandle disableHandle{};

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGC_Enemy Damage", meta = (AllowPrivateAccess = true))
	float damageInterval{ 0.1f };

	UPROPERTY()
	TSet<AIGC_Player*> overlappedPlayer{};

	FTSTicker::FDelegateHandle damageHandle{};

public:
	AIGC_Enemy(const FObjectInitializer& _Initializer);
	void BeginPlay()override;
	void InitEnemy();
	void SetActive(bool _bIsActive);
	bool ApplyDamage(float _DeltaTime);

private:
	void OnChangeState(E_CHARACTER_STATE _NewState);
	void OnDelay_ChangeDisable();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex);
};
