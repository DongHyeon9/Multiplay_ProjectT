#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "IG_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UIGW_Main;

UCLASS()
class PROJECT_T_API AIG_PlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	// 인게임 메인 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIG_PlayerController", meta = (AllowPrivateAccess = true))
	TSubclassOf<UIGW_Main> mainWidgetClass{};

	// 인게임 메인 위젯 오브젝트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIGW_Main> mainWidget{};

public:
	AIG_PlayerController(const FObjectInitializer& _Initializer);
	// 플레이어의 초기화를 진행
	// @param _NewData 플레이어의 데이터
	void InitPlayer(const FCharacterData& _NewData);
	// 서버에 플레이어의 초기화를 요청
	// @param _NewName 현재 접속한 플레이어의 이름
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InitPlayer(const FString& _NewName);
	void Server_InitPlayer_Implementation(const FString& _NewName);
	bool Server_InitPlayer_Validate(const FString& _NewName);

	// 서버에 시작 위젯 이벤트가 끝났다고 알림(게임 시간 동기화)
	UFUNCTION(Server, Reliable)
	void Server_OnFinishStartEvent();
	void Server_OnFinishStartEvent_Implementation();

	// 클라이언트에 게임을 시작한다
	UFUNCTION(Client, Reliable)
	void Client_StartGame();
	void Client_StartGame_Implementation();

	// 클라이언트에 게임 시작 애니메이션을 재생한다
	UFUNCTION(Client, Reliable)
	void Client_StartEvent();
	void Client_StartEvent_Implementation();

	// 클라이언트에 게임종료 애니메이션을 재생한다
	UFUNCTION(Client, Reliable)
	void Client_EndEvent();
	void Client_EndEvent_Implementation();

protected:
	void BeginPlay()override;

private:
	UFUNCTION()
	void PrintResult();
};
