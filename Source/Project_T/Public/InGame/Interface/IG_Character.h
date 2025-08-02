#pragma once

#include "../Project_T.h"
#include "UObject/Interface.h"
#include "IG_Character.generated.h"

class UIG_StatComponent;
class UWidgetComponent;
class AController;

UINTERFACE(MinimalAPI)
class UIG_Character : public UInterface
{
	GENERATED_BODY()
public:
	static const FName STAT_COMP_NAME;
	static const FName STAT_WIDGET_NAME;
};

class PROJECT_T_API IIG_Character
{
	GENERATED_BODY()
public:
	// 캐릭터의 스탯 컴포넌트를 가져온다
	virtual TObjectPtr<UIG_StatComponent> GetStatComp()const abstract;
	// 캐릭터의 상태 위젯을 가져온다
	virtual TObjectPtr<UWidgetComponent> GetStatusWidget()const abstract;

	// 대상에게 데미지를 준다
	// @param[in] _Instigator 가해자의 컨트롤러
	// @param[in] _Actor 가해자가 사용한 무기
	// @param[in] _Damage 가해지는 데미지량
	virtual void ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage);
};
