#include "InGame/Component/PlayerController/IGM_Screen.h"
#include "InGame/Player/IG_PlayerController.h"
#include "InGame/Player/IGC_Player.h"

void UIGM_Screen::ComputeScreenRadius()
{
	auto pc{ GetPlayerController() };

	const float traceDistance{ 10000.0f };
	UWorld* world{ GetWorld() };
	FHitResult outResult{};
	int32 xSize{}, ySize{};
	pc->GetViewportSize(xSize, ySize);

	//스크린 중앙의 월드좌표를 구한다
	FVector worldLocationM{}, worldDirectionM{};
	pc->DeprojectScreenPositionToWorld(xSize * 0.5f, ySize * 0.5f, worldLocationM, worldDirectionM);
	FVector targetLocation{ worldDirectionM * traceDistance + worldLocationM };
	if (world->LineTraceSingleByObjectType(
		outResult,
		worldLocationM,
		targetLocation,
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
	{
		worldLocationM = outResult.ImpactPoint;
	}

	//스크린 좌측 상단의 월드좌표를 구한다
	FVector worldLocationTL{}, worldDirectionTL{};
	pc->DeprojectScreenPositionToWorld(0, 0, worldLocationTL, worldDirectionTL);
	targetLocation = worldDirectionTL * traceDistance + worldLocationTL;
	if (world->LineTraceSingleByObjectType(
		outResult,
		worldLocationTL,
		targetLocation,
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic)))
	{
		worldLocationTL = outResult.ImpactPoint;
	}

	//반지름을 저장한다
	screenRadius = static_cast<float>(abs((worldLocationM - worldLocationTL).SquaredLength()));
}

void UIGM_Screen::InitScreenManager()
{
	FViewport::ViewportResizedEvent.AddUObject(this, &UIGM_Screen::SendViewportRadius);
	ComputeScreenRadius();
	Server_SendScreenRadius(screenRadius);
}

void UIGM_Screen::Server_SendScreenRadius_Implementation(float _ScreenRadius)
{
	screenRadius = _ScreenRadius;
}

void UIGM_Screen::SendViewportRadius(FViewport* _Viewport, uint32)
{
	if (!_Viewport) return;
	ComputeScreenRadius();
	Server_SendScreenRadius(screenRadius);
}


