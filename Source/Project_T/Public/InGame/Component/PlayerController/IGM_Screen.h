#pragma once

#include "IG_ManagerBase.h"
#include "IGM_Screen.generated.h"


UCLASS(meta = (BlueprintSpawnableComponent))
class PROJECT_T_API UIGM_Screen : public UIG_ManagerBase
{
	GENERATED_BODY()

public:

protected:

private:
	float screenRadius{};

public:	
	void ComputeScreenRadius();
	void InitScreenManager();

	FORCEINLINE float GetScreenRadius()const { return screenRadius; }

private:
	// 서버측에 내 스크린 정보를 보내준다
	UFUNCTION(Server, Reliable)
	void Server_SendScreenRadius(float _ScreenRadius);
	void Server_SendScreenRadius_Implementation(float _ScreenRadius);

	void SendViewportRadius(FViewport* _Viewport, uint32);
};
