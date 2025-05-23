#pragma once

#include "../Project_T.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "ILW_Main.generated.h"

class UButton;
class UEditableText;
class UILW_Modal;

UCLASS()
class PROJECT_T_API UILW_Main : public UUserWidget
{
	GENERATED_BODY()
private:
	// 매치 시작 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_StartMatch{};

	// 게임 종료 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_Quit{};

	// 플레이어의 이름을 적을 수 있는 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UEditableText> edt_Name{};

private:
	// 안내 문구를 띄워줄 모달 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UILW_Main", meta = (AllowPrivateAccess = true))
	TSubclassOf<UILW_Modal> modalWidgetClass{};

	// 모달위젯 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Main", meta = (AllowPrivateAccess = true))
	TObjectPtr<UILW_Modal> modalWidget{};

	// Online Subsystem 세션 인터페이스
	IOnlineSessionPtr sessionInterface{};

	// 세션 탐색 설정
	TSharedPtr<FOnlineSessionSearch> sessionSearch{};

	// 델리게이트 핸들
	FDelegateHandle findSessionsComplete{};

public:
	UILW_Main(const FObjectInitializer& _Initializer);
	void NativeConstruct()override;
	void NativeDestruct()override;

private:
	// 모달 위젯을 생성한다
	// @param _GuideText 안내문구
	// @param _ButtonText 버튼의 텍스트, 미입력시 "확인"으로 출력됨
	void CreateModal(const FText& _GuideText, const FText& _ButtonText = FText::FromString(TEXT("확인")));
	void RemoveModal();

	UFUNCTION()
	void OnClickedMatchCancel();

	UFUNCTION()
	void OnClickedStartMatch();

	UFUNCTION()
	void OnClickedQuit();

	UFUNCTION()
	void OnFindSessionsComplete(bool _bWasSuccessful);	

	void OnJoinSessionComplete(FName _SessionName, EOnJoinSessionCompleteResult::Type _Result);
};
