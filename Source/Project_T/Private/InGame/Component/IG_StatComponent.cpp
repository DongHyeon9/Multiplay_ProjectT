#include "InGame/Component/IG_StatComponent.h"
#include "Net/UnrealNetwork.h"

UIG_StatComponent::UIG_StatComponent(const FObjectInitializer& _Initializer)
	:Super(_Initializer)
{
	SetIsReplicatedByDefault(true);
}

void UIG_StatComponent::InitStat(int32 _Idx)
{
	SERVER_ONLY_COMP();
	SetDataTable(_Idx);
	SetHP(characterData->maxHP);
	SetCharacterState(E_CHARACTER_STATE::ENABLE);
}

void UIG_StatComponent::SetDataTable(int32 _Idx)
{
	SERVER_ONLY_COMP();
	checkf(statDataTable, TEXT("In Valid DataTable"));
	TArray<FName> rowNames{ statDataTable->GetRowNames() };
	checkf(rowNames.IsValidIndex(_Idx), TEXT("In Valid Row Data : %d"), _Idx);
	rowName = rowNames[_Idx];
	BROAD_CAST_CHANGE(onDataChange, characterData, statDataTable->FindRow<FCharacterData>(rowName, TEXT("")));
}

void UIG_StatComponent::SetCharacterState(E_CHARACTER_STATE _NewState)
{
	SERVER_ONLY_COMP();
	BROAD_CAST_CHANGE(onStateChange, characterState, _NewState);
}

void UIG_StatComponent::SetHP(float _NewHP)
{
	SERVER_ONLY_COMP();

	auto prev{ currentHP };
	currentHP = FMath::Clamp(_NewHP, 0.0f, characterData->maxHP);
	onHPChange.Broadcast(prev, currentHP);
}

void UIG_StatComponent::AddHP(float _AddHP)
{
	SetHP(currentHP + _AddHP);
}

void UIG_StatComponent::ApplyDamage(AController* _Instigator, AActor* _Actor, float _Damage)
{
	SERVER_ONLY_COMP();
	UE_LOG(LogTemp, Warning, TEXT("Damage %s -> %s : %.1f"), *_Actor->GetName(), *GetOwner()->GetName(), _Damage);
	AddHP(-_Damage);
	if (currentHP > 0.0f && !FMath::IsNearlyZero(currentHP)) return;

	SetCharacterState(E_CHARACTER_STATE::DEAD);
}

void UIG_StatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIG_StatComponent, characterState);
	DOREPLIFETIME(UIG_StatComponent, currentHP);
	DOREPLIFETIME(UIG_StatComponent, rowName);
}

void UIG_StatComponent::OnRep_StateChange(E_CHARACTER_STATE _Prev)
{
	onStateChange.Broadcast(_Prev, characterState);
}

void UIG_StatComponent::OnRep_HPChange(float _Prev)
{
	onHPChange.Broadcast(_Prev, currentHP);
}

void UIG_StatComponent::OnRep_DataChange(FName _Prev)
{
	characterData = statDataTable->FindRow<FCharacterData>(rowName, TEXT(""));
	auto prevData = statDataTable->FindRow<FCharacterData>(_Prev, TEXT(""));
	onDataChange.Broadcast(prevData, characterData);
}