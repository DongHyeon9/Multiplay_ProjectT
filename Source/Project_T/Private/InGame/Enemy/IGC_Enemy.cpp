#include "InGame/Enemy/IGC_Enemy.h"
#include "InGame/IG_StatComponent.h"

AIGC_Enemy::AIGC_Enemy(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
}

void AIGC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	GetStatComp()->onChangeState.AddUObject(this, &AIGC_Enemy::OnChangeState);
}

void AIGC_Enemy::InitEnemy()
{
	// TODO
	// 적 초기화 로직
}

void AIGC_Enemy::SetActive(bool _bIsActive)
{
	// TODO
	// 적 활성/비활성 로직
}

void AIGC_Enemy::OnChangeState(E_CHARACTER_STATE _NewState)
{
	PTT_LOG(Warning, TEXT("%s : %s"), *GetName(), *GET_ENUM_DISPLAY_NAME(E_CHARACTER_STATE, _NewState));

	switch (_NewState)
	{
	case E_CHARACTER_STATE::DISABLE: {

		break;
	}
	case E_CHARACTER_STATE::ENABLE: {

		break;
	}
	case E_CHARACTER_STATE::DEAD: {

		break;
	}
	}

	onEnemyState.ExecuteIfBound(this, _NewState);
}
