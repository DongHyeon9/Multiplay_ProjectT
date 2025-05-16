#pragma once

#include "Lobby/Widget/L_UserWidget.h"
#include "LW_MatchWait.generated.h"

class UButton;
class UHorizontalBox;
class ULW_ConnectState;
class APlayerController;

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
	TMap<APlayerController*, ULW_ConnectState*> connectStateMap{};

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	TSubclassOf<ULW_ConnectState> connectStateClass{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ULW_MatchWait", meta = (AllowPrivateAccess = true))
	float connectStateSize{ 200.0f };

public:
	void Init(int32 _MaxPlayerCount, const TArray<APlayerController*>& _PrevPlayers);
	void OnLoginPlayer(const APlayerController* _Player);
	void OnLogoutPlayer(const APlayerController* _Player);

};
