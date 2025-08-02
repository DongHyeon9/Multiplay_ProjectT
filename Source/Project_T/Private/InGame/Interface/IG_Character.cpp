#include "InGame/Interface/IG_Character.h"
#include "InGame/Component/IG_StatComponent.h"

const FName UIG_Character::STAT_COMP_NAME{ TEXT("StatComp") };
const FName UIG_Character::STAT_WIDGET_NAME{ TEXT("StatWidget") };

void IIG_Character::ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage)
{
	GetStatComp()->ApplyDamage(_Instigator, _Actor, _Damage);
}
