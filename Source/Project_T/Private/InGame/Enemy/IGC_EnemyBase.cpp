#include "InGame/Enemy/IGC_EnemyBase.h"
#include "InGame/IG_StatComponent.h"

AIGC_EnemyBase::AIGC_EnemyBase(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{
}

void AIGC_EnemyBase::BeginPlay()
{
	Super::BeginPlay();

	GetStatComp()->onChangeState.AddUObject(this, &AIGC_EnemyBase::OnChangeState);
}

void AIGC_EnemyBase::OnChangeState(E_CHARACTER_STATE _NewState)
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
