#pragma once

#include "Lobby/Widget/L_UserWidget.h"
#include "Containers/Ticker.h"
#include "LW_MatchWait.generated.h"

class UButton;
class UHorizontalBox;
class ULW_ConnectState;
class AL_PlayerController;

UCLASS()
class PROJECT_T_API ULW_MatchWait : public UL_UserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_MatchWait Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> btn_Cancel{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_MatchWait Default", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UHorizontalBox> hb_PlayerList{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "ULW_MatchWait Default", meta = (AllowPrivateAccess = true))
	TArray<ULW_ConnectState*> connectStateList{};

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	TSubclassOf<ULW_ConnectState> connectStateClass{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	float connectStateSize{ 200.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	float refreshInterval{ 0.1f };

	FTSTicker::FDelegateHandle refreshHandle{};

public:
	void NativeConstruct()override;
	void RemoveFromParent()override;
	void Init(int32 _MaxPlayerCount);
	bool RefreshMatchState(float _DeltaTime);

private:
	UFUNCTION()
	void OnClickedCancel();
};
