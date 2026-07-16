// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Game/LoadScreenSaveGame.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;

// 声明一个多播委托，该委托接受一个参数，参数类型为const FGameplayTagContainer&
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*资产标签*/);
// 技能初始化应用后的回调委托
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
// 单播委托，只能绑定一个回调
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);//单播委托，只能绑定一个回调
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, int32 /*技能等级*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, const FGameplayTag& /*输入标签*/, const FGameplayTag& /*上一个输入标签*/);
// 中止一个技能激活的回调
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*技能标签*/); //中止一个技能激活的回调
// 被动技能特效监听委托，对应特效是否开启
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag& /*被动技能标签*/, bool /*激活或取消*/); //被动技能特效监听委托，对应特效是否开启

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	// 技能初始化应用后的回调委托
	FAbilitiesGiven AbilitiesGivenDelegate;
	// 技能状态更新委托
	FAbilityStatusChanged AbilityStatusChanged;
	// 技能装配更新回调
	FAbilityEquipped AbilityEquipped;
	// 取消技能激活的委托
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	// 被动技能对应特效委托
	FActivatePassiveEffect ActivatePassiveEffect;
	
	/**
	 * 从存档读取技能数据设置角色技能
	 * @param SaveGameData 需要读取的存档
	 */
	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveGameData);
	/**
	 * 添加角色技能。
	 *
	 * @param StartupAbilities 一个包含要添加的技能类别的数组。
	 * 
	 * 此函数遍历提供的技能类别数组，并为每个类别创建一个技能规格。
	 * 如果技能是 `UAuraGameplayAbility` 的实例，则将技能的启动输入标签添加到技能规格的动态标签中。
	 * 然后，函数将技能规格添加到角色的技能系统中，但不立即激活它。
	 * 
	 * @note 此函数不会激活技能，只会添加技能到系统中。
	 */
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	// 应用被动技能
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	//初始化应用技能后，此值将被设置为true，用于记录当前是否被初始化完成
	bool bStartupAbilitiesGiven = false;
	/**
	 * 处理按住技能输入标签的事件。
	 *
	 * @param InputTag 与技能输入关联的游戏标签。
	 * 
	 * 此函数检查提供的输入标签是否有效。如果有效，它会锁定技能列表并遍历所有可激活的技能。
	 * 对于每个技能，如果技能具有与输入标签完全相同的动态标签，则将技能输入标记为已按下。
	 * 如果技能未激活，则尝试激活该技能。
	 */ 
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	
	/**
	 * 处理按住技能输入标签的事件。
	 *
	 * @param InputTag 与技能输入关联的游戏标签。
	 * 
	 * 此函数检查提供的输入标签是否有效。如果有效，它会锁定技能列表并遍历所有可激活的技能。
	 * 对于每个技能，如果技能具有与输入标签完全相同的动态标签，则将技能输入标记为已按下。
	 * 如果技能未激活，则尝试激活该技能。
	 */
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	/**
	 * 处理释放技能输入标签的事件。
	 *
	 * @param InputTag 与技能输入关联的游戏标签。
	 * 
	 * 此函数检查提供的输入标签是否有效。如果有效，它会遍历所有已经应用的技能。
	 * 对于每个技能，如果技能具有与输入标签完全相同的动态标签，则将技能输入标记为已释放。
	 * 如果技能处于激活状态，则尝试停止该技能。
	 */
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	// 遍历技能，并将技能广播出去
	void ForEachAbility(const FForEachAbility& Delegate);
	
	// 获取技能的标签
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	// 获取技能的输入标签
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	// 获取技能的状态标签
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	/**
	 * 判断技能槽位是否装配了技能
	 * @param Slot 技能槽位设置的技能输入标签 
	 * @return 如果槽位有技能，返回false,如果槽位是空的返回true
	 */
	bool SlotIsEmpty(const FGameplayTag& Slot);

	/**
	 * 判断技能设置的槽位
	 * @param Spec 需要判断的技能实例
	 * @param Slot 需要判断的槽位的输入标签
	 * @return 如果当前技能的输入标签和槽位的标签一致，返回true
	 */
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);

	/**
	 * 判断技能是否装配到技能槽位上
	 * @param Spec 需要判断的技能
	 * @return 如果装配到了技能槽位，返回true
	 */
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	
	/**
	 * 通过插槽的技能输入标签获取技能实例
	 * @param Slot 插槽的输入标签
	 * @return 技能实例
	 */
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);

	/**
	 * 判断技能是否为被动技能
	 * @param Spec 技能实例
	 * @return 如果技能为被动技能，返回true
	 */
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;
	
	/**
	 * 将技能应用到目标槽位
	 * @param Spec 目标技能
	 * @param Slot 需要应用到的槽位
	 */
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);

	/**
	 * 多网络被动特效委托广播，让每个客户端都可以看到特效
	 * @param AbilityTag 被动技能标签
	 * @param bActivate 激活或者关闭
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	// 根据技能标签获取该技能的状态标签（如"已装备"、"可解锁"等）
	FGameplayTag GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag);
	
	// 根据技能标签获取该技能的输入槽位标签（如对应按键的输入标签）
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	
	// 通过技能标签获取技能实例
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	// 升级属性
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
	// 服务器升级属性函数
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	
	// 根据角色等级更新技能数据状态
	void UpdateAbilityStatuses(int32 Level);
	
	// 只在服务器端运行，消耗技能点函数提升技能等级
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);
	
	// 在服务器处理技能装配，传入技能标签和装配的技能标签
	UFUNCTION(Server, Reliable) 
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot); 

	// 在客户端处理技能装配
	UFUNCTION(Client, Reliable) 
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	//清除技能装配插槽的技能
	static void ClearSlot(FGameplayAbilitySpec* Spec); 

	// 根据输入标签，清除技能装配插槽的技能
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
	

	
	// 判断当前技能实例是否处于目标技能装配插槽
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);

	
	// 通过技能标签获取技能描述信息
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

protected:

	//在ASC里面我们实现了一个EffectApplied的函数，在GE被应用的时候，会触发此函数回调，用于显示被使用的是什么物品。然后在EffectApplied函数中使用自定义委托广播出去，在WidgetController中监听自定义委托。但是，现在OnGameplayEffectAppliedDelegateToSelf无法在客户端运行。
	//效果应用时的委托（废用）
	//void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	//标记Client告诉UE这个函数应该只在客户端运行，设置Reliable表示这个函数调用是可靠的，即它确保数据能够到达客户端
	//UFUNCTION(Client, Reliable)
	//void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) const;
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) const;
	// 技能状态更新后回调
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
};
