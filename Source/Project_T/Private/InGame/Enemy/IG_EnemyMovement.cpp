#include "InGame/Enemy/IG_EnemyMovement.h"

UIG_EnemyMovement::UIG_EnemyMovement(const FObjectInitializer& _Initializer)
	: Super(_Initializer)
{

}

void UIG_EnemyMovement::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);
}
