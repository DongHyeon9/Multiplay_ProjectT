#pragma once

#include "Intro/Widget/IL_UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "ILW_Main.generated.h"

class UButton;
class UEditableText;
class UILW_Modal;

UCLASS()
class PROJECT_T_API UILW_Main : public UIL_UserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_StartMatch{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_Quit{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "UILW_Main Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UEditableText> edt_Name{};

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "UILW_Main", meta = (AllowPrivateAccess = true))
	TSubclassOf<UILW_Modal> modalWidgetClass{};

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

private:
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
