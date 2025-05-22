#pragma once

#include "InGame/Skill/IG_SkillBase.h"
#include "IGSB_Spin.generated.h"

class UShapeComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECT_T_API AIGSB_Spin : public AIG_SkillBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	uint8 currentSpin{ 1 };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	float radius{ 100.f };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	float speed{ 1 };

protected:

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	TObjectPtr<UShapeComponent> centerMesh{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> spinMesh1{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> spinMesh2{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> spinMesh3{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> spinMesh4{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AIGSB_Spin", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> spinMesh5{};


	TArray<TObjectPtr<UStaticMeshComponent>>spinMeshArray{};
	uint8 MaxSpin{ 5 };
public:
	AIGSB_Spin();
	void BeginPlay() override;
	virtual void InitSkill();
	virtual void Tick(float _DeltaTime);

	void UseSkill() override;

};
