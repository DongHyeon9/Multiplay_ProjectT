#include "InGame/Skill/IGSB_LineTrace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "T_GameInstance.h"

void AIGSB_LineTrace::InitSkill()
{
	this->SetOwner(GetOwner());
	skillName = TEXT("LineTrace");
	level = 1;
	duration = 0.5f;
	coolDown = 5.f;
	PTT_LOG(Warning, TEXT("skillName : %s\nlevel : %d\nduration : %0.1f\ncoolDown : %0.1f"), *skillName.ToString(), level, duration, coolDown);
	Super::InitSkill();

}

void AIGSB_LineTrace::UseSkill()
{
	FHitResult Hit;
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocation = StartLocation + (GetOwner()->GetActorForwardVector() * 1000.f);

	bool Result = UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		FVector(10.f, 10.f, 10.f),
		FRotator(0.f, 0.f, 0.f),
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		{
			GetOwner()
		},
		EDrawDebugTrace::ForDuration,
		Hit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.f
	);
	PTT_LOG(Warning, TEXT("Use LineTrace"));
	
}
