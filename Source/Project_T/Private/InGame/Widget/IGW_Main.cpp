#include "InGame/Widget/IGW_Main.h"
#include "InGame/Player/IGC_Player.h"
#include "InGame/Component/Player/IG_PlayerStatComponent.h"
#include "InGame/IG_GameState.h"
#include "InGame/Widget/IGW_SkillCard.h"
#include "InGame/Widget/IGW_SkillSelect.h"
#include "InGame/Player/IG_PlayerController.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UIGW_Main::NativeConstruct()
{
	Super::NativeConstruct();

	if (!wg_SkillSelect->onSelectedSkill.IsBoundToObject(this))
		wg_SkillSelect->onSelectedSkill.BindUObject(this, &UIGW_Main::OnSelectedSkill);
}

void UIGW_Main::InitWidget(TObjectPtr<AIG_GameState> _GameState)
{
	BindToAnimationFinished(startWidgetAnim, onFinishStartAnim);
	BindToAnimationFinished(endWidgetAnim, onFinishEndAnim);
	pb_HPBar->SetPercent(1.0f);
	pb_EXPBar->SetPercent(0.0f);

	gameState = _GameState;
	txt_CurrentTime->SetVisibility(ESlateVisibility::Visible);
}

void UIGW_Main::SetName(FText _Name)
{
	txt_Name->SetText(_Name);
}

void UIGW_Main::SetName(const FString& _Name)
{
	txt_Name->SetText(FText::FromString(_Name));
}

void UIGW_Main::StartGame()
{
	PlayAnimation(startWidgetAnim);
}

void UIGW_Main::EndGame()
{
	txt_CurrentTime->SetVisibility(ESlateVisibility::Hidden);
	PlayAnimation(endWidgetAnim);
}

void UIGW_Main::InitSkillSelect(const TArray<FSkillInfoParam>& _SkillInfo)
{
	skillCardInfo.Empty();
	for (auto skillInfo : _SkillInfo)
	{
		auto cardInfo{ NewObject<UIG_SkillCardInfo>(GetOwningPlayer()) };
		cardInfo->InitSkillItem(skillInfo);
		skillCardInfo.Emplace(cardInfo);
	}
	wg_SkillSelect->InitializeSkillSelect(skillCardInfo);
	wg_SkillSelect->Appearance();
}

void UIGW_Main::NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)
{
	Super::NativeTick(_MyGeometry, _InDeltaTime);

	//시간을 갱신한다
	if (gameState)
	{
		txt_CurrentTime->SetText(FText::FromString(FString::Printf(TEXT("남은 시간 : %d"), gameState->GetTimer())));
	}
}

void UIGW_Main::OnChangeHP(float _Prev, float _Current)
{
	check(playerStat);
	auto playerData{ playerStat->GetCharacterData<FCharacterData_Player>() };
	pb_HPBar->SetPercent(_Current <= 0.0f ? 0.0f : _Current / playerData->maxHP);
}

void UIGW_Main::OnChangeEXP(float _Prev, float _Current)
{
	check(playerStat);
	auto playerData{ playerStat->GetCharacterData<FCharacterData_Player>() };
	pb_EXPBar->SetPercent(playerData->needEXP <= 0.0f ? 0.0f : _Current / playerData->needEXP);
}

void UIGW_Main::OnSelectedSkill(FName _SkillName)
{
	auto pc{ GetOwningPlayer<AIG_PlayerController>() };
	check(pc);
	pc->SetShowMouseCursor(false);
	pc->SetInputMode(FInputModeGameOnly{});
	pc->Server_SkillAcquisition(_SkillName);
}

void UIGW_Main::BindStat(TObjectPtr<UIG_PlayerStatComponent> _StatComp)
{
	check(_StatComp);
	playerStat = _StatComp;
	if (!playerStat->onEXPChange.IsBoundToObject(this))
		playerStat->onEXPChange.AddUObject(this, &UIGW_Main::OnChangeEXP);
	if (!playerStat->onHPChange.IsBoundToObject(this))
		playerStat->onHPChange.AddUObject(this, &UIGW_Main::OnChangeHP);

	auto playerData{ playerStat->GetCharacterData<FCharacterData_Player>() };
	pb_HPBar->SetPercent(playerStat->GetCurrentHP() / playerData->maxHP);
	pb_EXPBar->SetPercent(playerStat->GetCurrentEXP() / playerData->needEXP);
}
