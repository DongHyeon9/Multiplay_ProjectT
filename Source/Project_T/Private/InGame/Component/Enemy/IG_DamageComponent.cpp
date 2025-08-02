#include "InGame/Component/Enemy/IG_DamageComponent.h"
#include "Components/ShapeComponent.h"
#include "InGame/Component/IG_StatComponent.h"

void UIG_DamageComponent::InitDamageComponent(const FDamageCompParameter& _Param)
{
	damageInfo = _Param;
}

void UIG_DamageComponent::ActiveDamageComp(bool _bIsActive)
{
	bIsActive = _bIsActive;
	ResetDamageComponent();
}

void UIG_DamageComponent::AddOverlapped(TObjectPtr<UIG_StatComponent> _NewOverlap)
{
	overlapped.Emplace(_NewOverlap);
	if (!_NewOverlap->onStateChange.IsBoundToObject(this))
		_NewOverlap->onStateChange.AddUObject(this, &UIG_DamageComponent::OnStateChange, _NewOverlap);
	if (damageHandle.IsValid()) return;
	damageHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UIG_DamageComponent::Ticker_ApplyDamage), damageInfo.damageInterval);
}

void UIG_DamageComponent::RemoveOverlapped(TObjectPtr<UIG_StatComponent> _RemoveOverlap)
{
	if (!overlapped.Contains(_RemoveOverlap)) return;

	overlapped.Remove(_RemoveOverlap);
	if (overlapped.IsEmpty())
	{
		ClearDamageHandle();
	}
}

void UIG_DamageComponent::ResetDamageComponent()
{
	overlapped.Empty();
	ClearDamageHandle();
}

void UIG_DamageComponent::BeginPlay()
{
	Super::BeginPlay();

	check(damageInfo.ownerCollision);
	damageInfo.ownerCollision->ComponentTags.Emplace(IG_COLLISION_TAG::ATTACK);

#ifndef WITH_EDITOR
	SERVER_ONLY_COMP();
#endif
	if (!damageInfo.ownerCollision->OnComponentBeginOverlap.IsAlreadyBound(this,&UIG_DamageComponent::OnBeginOverlap))
		damageInfo.ownerCollision->OnComponentBeginOverlap.AddDynamic(this, &UIG_DamageComponent::OnBeginOverlap);
	
	if (!damageInfo.ownerCollision->OnComponentEndOverlap.IsAlreadyBound(this, &UIG_DamageComponent::OnEndOverlap))
		damageInfo.ownerCollision->OnComponentEndOverlap.AddDynamic(this, &UIG_DamageComponent::OnEndOverlap);
}

void UIG_DamageComponent::EndPlay(EEndPlayReason::Type _Reason)
{
	Super::EndPlay(_Reason);

	if (damageHandle.IsValid())
		FTSTicker::GetCoreTicker().RemoveTicker(damageHandle), damageHandle.Reset();
}

bool UIG_DamageComponent::Ticker_ApplyDamage(float _DeltaTime)
{
	if (!bIsActive) return false;

	SERVER_ONLY_COMP(false);

	auto owner{ GetOwner() };
	if (!owner) return false;
	auto controller{ owner->GetInstigatorController() };
	if (!controller) return false;
	auto ownerStat{ owner->GetComponentByClass<UIG_StatComponent>() };
	if (!ownerStat) return false;

	for (auto otherStat : overlapped)
	{
		if (!otherStat) continue;
		if (otherStat->GetCharacterState() != E_CHARACTER_STATE::ENABLE) continue;

		otherStat->ApplyDamage(controller, owner, ownerStat->GetDamage());
	}
	return true;
}

void UIG_DamageComponent::ClearDamageHandle()
{
	if (!damageHandle.IsValid()) return;

	FTSTicker::GetCoreTicker().RemoveTicker(damageHandle);
	damageHandle.Reset();
}

void UIG_DamageComponent::OnBeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	if (!bIsActive) return;
	if (!_OtherComp->ComponentHasTag(IG_COLLISION_TAG::CHARACTER)) return;

	auto statComp{ _OtherActor->FindComponentByClass<UIG_StatComponent>() };
	if (!statComp) return;
	AddOverlapped(statComp);
}

void UIG_DamageComponent::OnEndOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex)
{
	if (!bIsActive) return;

	auto statComp{ _OtherActor->FindComponentByClass<UIG_StatComponent>() };
	if (!statComp) return;
	RemoveOverlapped(statComp);
}

void UIG_DamageComponent::OnStateChange(E_CHARACTER_STATE _Prev, E_CHARACTER_STATE _Current, TObjectPtr<UIG_StatComponent> _Target)
{
	if (_Current != E_CHARACTER_STATE::ENABLE) return;
	RemoveOverlapped(_Target);
}
