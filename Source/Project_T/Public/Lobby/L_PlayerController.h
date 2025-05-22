#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "L_PlayerController.generated.h"

class ULW_MatchWait;

UCLASS()
class PROJECT_T_API AL_PlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	// 로비의 메인 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AL_PlayerController", meta = (AllowPrivateAccess = true))
	TSubclassOf<ULW_MatchWait> matchWaitClass{};

	// 메인 위젯의 오브젝트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AL_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<ULW_MatchWait> matchWaitWidget{};

public:
	void BeginPlay()override;

	// 모든 클라이언트에 표시되는 이름을 설정한다
	// @param _UserName 설정할 플레이어 이름
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerName(const FString& _UserName);
	void Server_SetPlayerName_Implementation(const FString& _UserName);
	bool Server_SetPlayerName_Validate(const FString& _UserName);
};
