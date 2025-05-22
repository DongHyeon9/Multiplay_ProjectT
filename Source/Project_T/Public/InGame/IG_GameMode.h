#pragma once

#include "../Project_T.h"
#include "GameFramework/GameMode.h"
#include "IG_GameMode.generated.h"

UCLASS()
class PROJECT_T_API AIG_GameMode : public AGameMode
{
	GENERATED_BODY()
private:
	// 플레이어마다 다른 색깔을 적용한다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameMode", meta = (AllowPrivateAccess = true))
	TArray<FColor> playerColor{ FColor::Red, FColor::Blue, FColor::Green, FColor::Purple, FColor::Orange };

	// 색깔을 배정받지 못한 플레이어의 기본 색깔
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_GameMode", meta = (AllowPrivateAccess = true))
	FColor defaultColor{ FColor::White };

public:
	AIG_GameMode(const FObjectInitializer& _Initializer);
	// 모든 플레이어가 입장에 성공하면 
	// 위치, 이름, 색깔을 설정 하고
	// 플레이어의 초기화를 진행한다
	void OnCompleteAllPlayer();
};
