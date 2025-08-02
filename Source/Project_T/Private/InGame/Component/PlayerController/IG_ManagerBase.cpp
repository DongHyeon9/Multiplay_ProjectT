#include "InGame/Component/PlayerController/IG_ManagerBase.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/Player/IG_PlayerController.h"

UIG_ManagerBase::UIG_ManagerBase(const FObjectInitializer& _Init)
	:Super(_Init)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UIG_ManagerBase::BeginPlay()
{
	Super::BeginPlay();

	playerController = GetOwner<AIG_PlayerController>();
	check(playerController);
	playerCharacter = playerController->GetPawn<AIGC_Player>();
	check(playerCharacter);
}
