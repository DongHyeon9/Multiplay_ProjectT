#pragma once

#include "../Project_T.h"
#include "GameFramework/Character.h"
#include "IG_CharacterBase.generated.h"

class UInputComponent;
class UIG_StatComponent;
class UWidgetComponent;

UCLASS(Abstract)
class PROJECT_T_API AIG_CharacterBase : public ACharacter
{
	GENERATED_BODY()
private:
	// 캐릭터의 스탯을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UIG_StatComponent> statComp{};

	// 현재 플레이어의 상태(이름, 체력 등)을 표시하는 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase Defulat", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> statusWidget{};

private:
	//캐릭터의 정보를 담는 구조체
	UPROPERTY(Transient, ReplicatedUsing = OnRep_UpdateCharacterData, VisibleAnywhere, BlueprintReadOnly, category = "AIG_CharacterBase", meta = (AllowPrivateAccess = true))
	FCharacterData characterData{};

public:
	AIG_CharacterBase(const FObjectInitializer& _Intializer);

public:
	// 캐릭터의 스탯 컴포넌트를 가져온다
	FORCEINLINE TObjectPtr<UIG_StatComponent> GetStatComp()const { return statComp; }
	// 캐릭터의 상태 위젯을 가져온다
	FORCEINLINE TObjectPtr<UWidgetComponent> GetStatusWidget()const { return statusWidget; }
	// 캐릭터의 정보를 가져온다
	FORCEINLINE const FCharacterData& GetCharacterData()const { return characterData; }
	// 캐릭터의 정보를 설정한다
	// @param _NewData 설정할 데이터
	virtual void SetCharacterData(const FCharacterData& _NewData);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_UpdateCharacterData();

};
