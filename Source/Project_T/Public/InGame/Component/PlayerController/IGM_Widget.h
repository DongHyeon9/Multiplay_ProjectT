#pragma once

#include "IG_ManagerBase.h"
#include "Animation/WidgetAnimationEvents.h"
#include "IGM_Widget.generated.h"

class UIGW_Main;
class UIGW_ResultPopup;
class UIG_PlayerStatComponent;

UCLASS(meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIGM_Widget : public UIG_ManagerBase
{
	GENERATED_BODY()

public:

protected:

private:
	// 인게임 메인 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGM_Widget", meta = (AllowPrivateAccess = true))
	TSubclassOf<UIGW_Main> mainWidgetClass{};

	// 결과창 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UIGM_Widget", meta = (AllowPrivateAccess = true))
	TSubclassOf<UIGW_ResultPopup> resultPopupClass{};

private:
	// 인게임 메인 위젯 오브젝트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGM_Widget", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIGW_Main> mainWidget{};

	// 인게임 메인 위젯 오브젝트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UIGM_Widget", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIGW_ResultPopup> resultWidget{};

public:	
	UIGM_Widget(const FObjectInitializer& _Init);

public:
	FORCEINLINE TObjectPtr<UIGW_Main> GetMainWidget()const { return mainWidget; }
	FORCEINLINE TObjectPtr<UIGW_ResultPopup> GetResultWidget()const { return resultWidget; }

public:
	FWidgetAnimationDynamicEvent& MainWidget_GetOnFinishStartAnim();

	//게임시작 애니메이션을 재생한다
	void MainWidget_StartGameAnim();
	//게임 종료 애니메이션을 재생한다
	void MainWidget_EndGameAnim();
	//위젯을 초기화 한다
	void MainWidget_Init();
	//위젯에 이름을 설정한다
	void MainWidget_SetName(const FString& _NewName);
	void MainWidget_BindStat(const TObjectPtr<UIG_PlayerStatComponent> _StatComp);
	UFUNCTION(Client, Reliable)
	void MainWidget_Client_SkillAcquisition(const TArray<FName>& _SkillList);
	void MainWidget_Client_SkillAcquisition_Implementation(const TArray<FName>& _SkillList);

protected:
	void BeginPlay() override;

private:
	UFUNCTION()
	void ShowGameResult();
};
