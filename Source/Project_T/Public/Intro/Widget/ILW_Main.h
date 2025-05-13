#pragma once

#include "Intro/Widget/IL_UserWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
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

	// Online Subsystem ���� �������̽�
	IOnlineSessionPtr sessionInterface{};
	// ���� Ž�� ����
	TSharedPtr<FOnlineSessionSearch> sessionSearch{};

	// ��������Ʈ �ڵ�
	FDelegateHandle findSessionsComplete{};

public:
	void NativeConstruct()override;

private:
	void CreateModal(const FText& _GuideText);

	UFUNCTION()
	void OnClickedStartMatch();

	UFUNCTION()
	void OnClickedQuit();

	UFUNCTION()
	void OnFindSessionsComplete(bool _bWasSuccessful);	
};
