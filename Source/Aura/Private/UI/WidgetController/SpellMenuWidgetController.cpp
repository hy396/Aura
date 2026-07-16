// Huanyu,miaomiaomiao,nekoneko


#include "UI/WidgetController/SpellMenuWidgetController.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// 绑定技能状态修改后的委托回调
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		// 当当前选中技能的状态发生变化时执行
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			// 更新选中技能的最新状态
			SelectedAbility.Status = StatusTag;
			// 初始化按钮状态标志
			bool bEnableSpendPoints = false; // 是否允许消耗技能点升级
			bool bEnableEquip = false;       // 是否允许装备技能
			
			// 根据新状态和当前技能点判断按钮可用性
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			
			// 存储技能描述的字符串
			FString Description;            // 当前等级技能描述
			FString NextLevelDescription;   // 下一等级技能描述
			// 获取技能详细描述信息
			GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
			// 广播更新UI元素：
			// - 升级/装备按钮状态
			// - 技能描述文本内容
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
		}
		
		if (AbilityInfo)
		{
			// 获取技能数据
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});
	// 监听技能装配的回调
	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	// 绑定技能点变动回调
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](const int32 SpellPoints)
	{
		SpellPointsChanged.Broadcast(SpellPoints); //广播拥有的技能点
		// 更新当前技能点
		CurrentSpellPoints = SpellPoints;

		// 初始化按钮状态标志
		bool bEnableSpendPoints = false; // 是否允许消耗技能点升级
		bool bEnableEquip = false;       // 是否允许装备技能
		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
		// 存储技能描述的字符串
		FString Description;            // 当前等级技能描述
		FString NextLevelDescription;   // 下一等级技能描述
		// 获取技能详细描述信息
		GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		// 广播更新UI元素：
		// - 升级/装备按钮状态
		// - 技能描述文本内容
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);

	});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
 	BroadcastAbilityInfo();
	SpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		// 通过技能标签获取对应技能的类型标签（如主动技能/被动技能）
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		// 广播停止等待装备选择事件，通知UI更新状态
		// 参数传递当前选择的技能类型，用于界面元素重置
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		// 重置等待装备选择标志位，结束装备选择流程
		bWaitingForEquipSelection = false;
	}
	
	const FMyGameplayTags GameplayTags = FMyGameplayTags::Get();
	// 获取技能点数
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	// 创建技能状态 
	FGameplayTag AbilityStatus;
	
	// 判断传入的标签是否存在
	const bool bTagValid = AbilityTag.IsValid();
	// 判断传入的是否为空技能标签
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	// 通过技能标签获取技能
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	// 判断技能实例是否存在
	const bool bSpecValid = AbilitySpec != nullptr;

	if (!bTagValid || bTagNone || !bSpecValid)
	{
		//传入标签不存在，或传入的为空技能标签，或者技能实例不存在时，设置为技能按钮显示上锁状态
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}else
	{
		// 从技能实例获取技能的状态标签
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
	}
	
	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	bool bEnableSpendPoints = false; //技能是否可以升级
	bool bEnableEquip = false; //技能是否可以装配
	// 获取结果
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
	// 存储技能描述的字符串
	FString Description;            // 当前等级技能描述
	FString NextLevelDescription;   // 下一等级技能描述
	// 获取技能详细描述信息
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	// 广播更新UI元素：
	// - 升级/装备按钮状态
	// - 技能描述文本内容
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}	


void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	// 如果当前正在等待装备选择
	if (bWaitingForEquipSelection)
	{
		// 获取当前选中能力的类型
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		// 广播停止等待装备选择的事件，并传递选中能力的类型
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		// 停止等待装备选择
		bWaitingForEquipSelection = false;
	}
	// 将选中的能力重置为 "无"
	SelectedAbility.Ability = FMyGameplayTags::Get().Abilities_None;
	// 将选中的能力状态重置为 "锁定"
	SelectedAbility.Status = FMyGameplayTags::Get().Abilities_Status_Locked;
	// 广播法术球取消选择事件，传递默认参数（未启用按钮、空描述等）
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	// 获取当前选中技能的类型（如主动技能/被动技能）
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

	// 广播装备槽位选择事件，触发UI显示可用装备槽位
	WaitForEquipDelegate.Broadcast(AbilityType);
	// 设置等待装备选择标志，进入装备槽位选择模式
	bWaitingForEquipSelection = true;
	// 获取当前技能的装备状态
	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusTagFromAbilityTag(SelectedAbility.Ability);
	// 如果当前已经是已装备状态（重新装备情况）
	if (SelectedStatus.MatchesTagExact(FMyGameplayTags::Get().Abilities_Status_Equipped))
	{
		// 记录当前已分配的装备槽位（用于后续可能的槽位替换）
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return;
	// 检查所选技能与技能槽的技能类型是否匹配
	// （例如：不要将攻击型法术装备到被动技能槽中，反之亦然）
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;
	//调用装配技能函数，进行处理
	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	// 停止等待装备选择
	bWaitingForEquipSelection = false;
	// 获取全局游戏标签
	const FMyGameplayTags GameplayTags = FMyGameplayTags::Get();

	// 清除旧插槽的数据
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked; // 设置状态为已解锁
	LastSlotInfo.InputTag = PreviousSlot; // 设置之前的槽位
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None; // 设置能力标签为无
	// 如果 PreviousSlot 是有效槽位，则广播空的能力信息（仅在装备已装备的法术时）
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	//更新新插槽的数据 查找当前装备的能力信息
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status; // 更新能力状态
	Info.InputTag = Slot; // 更新槽位
	AbilityInfoDelegate.Broadcast(Info); // 广播更新后的能力信息
	
	// 停止等待装备选择，并广播能力类型
	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);

	// 广播法术球重新分配事件
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);

	// 取消选择当前法术球
	GlobeDeselect();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
	bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const FMyGameplayTags GameplayTags = FMyGameplayTags::Get();

	// 初始化按钮状态为不可用
	bShouldEnableSpellPointsButton = false;
	bShouldEnableEquipButton = false;

	// 状态判断：技能已装备状态
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		// 已装备技能允许重新配置装备位置
		bShouldEnableEquipButton = true;
		// 当有剩余技能点时允许升级
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	// 状态判断：技能符合解锁条件但未解锁
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		// 仅当有技能点时允许解锁（升级）
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	// 状态判断：技能已解锁但未装备
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		// 允许装备该技能
		bShouldEnableEquipButton = true;
		// 同时有技能点时允许继续升级
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
}
