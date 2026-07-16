// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "MyGameplayTags.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

//在技能面板选中的技能的标签结构体
struct FSelectedAbility
{
	// 技能标签
	FGameplayTag Ability = FGameplayTag();
	// 技能状态标签
	FGameplayTag Status = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:

	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
	// 蓝图可分配的委托，用于通知技能点数变化
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChanged;
	
	//选中技能按钮后，升级和装备按钮的变动回调
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;
	
	// 等待装备选择状态开始委托（参数：技能类型标签）
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;
	
	// 结束等待装备选择状态委托（参数：技能类型标签）
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;
	
	// 技能球重新分配委托（参数：技能标签）
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;
	
	
	// 技能按钮选中调用函数，处理升级按钮和装配
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);
	
	// 点击消耗技能点数按钮时调用
	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();
	
	// 取消当前选中的技能球
	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();
	
	//装配技能按钮按下事件
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();
	
	// 技能栏位点击处理（参数：装备槽位标签，技能类型标签）
	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);
	
	// 技能装备完成回调（参数：技能标签，状态，新槽位，旧槽位）
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

private:
	// 通过技能状态标签和可分配技能点数来获取技能是否可以装配和技能是否可以升级
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);

	// 当前选中的技能信息（包含技能标签和状态）
	FSelectedAbility SelectedAbility = { FMyGameplayTags::Get().Abilities_None,  FMyGameplayTags::Get().Abilities_Status_Locked };

	// 当前持有的技能点数
	int32 CurrentSpellPoints = 0;

	// 是否处于等待装备选择状态
	bool bWaitingForEquipSelection = false;
	
	// 当前选中的装备槽位标签
	FGameplayTag SelectedSlot;

};
