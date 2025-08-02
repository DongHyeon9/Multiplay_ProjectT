#pragma once

#include "../Project_T.h"
#include "GameFramework/PlayerController.h"
#include "IG_PlayerController.generated.h"

class UIGM_Widget;
class UIGM_Screen;

UCLASS()
class PROJECT_T_API AIG_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	static const FName WIDGET_MANAGER_NAME;
	static const FName SCREEN_MANAGER_NAME;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIGM_Widget> widgetManager{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_PlayerController", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIGM_Screen> screenManager{};

public:
	AIG_PlayerController(const FObjectInitializer& _Initializer);
	// 플레이어의 초기화를 진행
	// @param _NewData 플레이어의 데이터
	void InitPlayer(const FPlayerData& _NewData);
	void OnEnterPlayer();
	void GameOver();
	void EndGame();

	FORCEINLINE TObjectPtr<UIGM_Widget> GetWidgetManager()const { return widgetManager; }
	FORCEINLINE TObjectPtr<UIGM_Screen> GetScreenManager()const { return screenManager; }

public:

#pragma region Server -> Client

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

#pragma endregion

#pragma region Client -> Server

	// 서버에 시작 위젯 이벤트가 끝났다고 알림(게임 시간 동기화)
	UFUNCTION(Server, Reliable)
	void Server_OnFinishStartEvent();
	void Server_OnFinishStartEvent_Implementation();

	// 서버에 플레이어의 초기화를 요청
	// @param _NewName 현재 접속한 플레이어의 이름
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnEnterPlayer(const FString& _NewName);
	void Server_OnEnterPlayer_Implementation(const FString& _NewName);
	bool Server_OnEnterPlayer_Validate(const FString& _NewName);

	// 클라이언트에 스킬을 배울 수 있는 UI를 출력한다
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SkillAcquisition(FName _Skill);
	void Server_SkillAcquisition_Implementation(FName _Skill);
	bool Server_SkillAcquisition_Validate(FName _Skill);

#pragma endregion

protected:
	void BeginPlay()override;

private:

};
