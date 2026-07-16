// Huanyu,miaomiaomiao,nekoneko


#include "UI/WidgetController/OverlayWidgetController.h"

#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"


void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	//绑定等级相关回调
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	//绑定等级相关回调
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel, bool bLevelUp)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});
	if (GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			// 如果执行到此处时，技能的初始化工作已经完成，则直接调用初始化回调
			BroadcastAbilityInfo();
		}else
		{
			// 如果执行到此处，技能初始化还未完成，将通过绑定委托，监听广播的形式触发初始化完成回调
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}
		//AddLambda 绑定匿名函数
		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (FGameplayTag Tag: AssetTags)
				{
					//对标签进行检测，如果不是信息标签，将无法进行广播
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					// "Message.HealthPotion".MatchesTag("Message") 将返回 True，
					// "Message".MatchesTag("Message.HealthPotion") 将返回 False
					if(Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("无法查询到等级相关数据，请查看PlayerState是否设置了对应的数据"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP); // 获取当前等级
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num(); //获取当前最大等级
	if (Level <= MaxLevel && Level > 0)
	{
		// 当前等级升级所需经验值
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		// 上一级升级所需经验值
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;

		// 返回经验进度条的分子和分母
		const int32 XPSinceLastLevel = NewXP - PreviousLevelUpRequirement;
		const int32 XPRequiredForCurrentLevel = LevelUpRequirement - PreviousLevelUpRequirement;
		// 计算经验百分比
		const float XPPercent = (static_cast<float>(XPSinceLastLevel) / static_cast<float>(XPRequiredForCurrentLevel));
		
		OnXPPercentChangedDelegate.Broadcast(XPSinceLastLevel, XPRequiredForCurrentLevel, XPPercent); //广播经验条比例
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	// 获取全局游戏标签
	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	
	// 清除旧插槽的数据
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked; // 设置状态为已解锁
	LastSlotInfo.InputTag = PreviousSlot; // 设置之前的槽位
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None; // 设置能力标签为无
	// 如果 PreviousSlot 是有效槽位，则广播空的能力信息（仅在装备已装备的法术时）
	AbilityInfoDelegate.Broadcast(LastSlotInfo);
	
	//更新新插槽的数据 查找当前装备的能力信息
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;	// 更新能力状态
	Info.InputTag = Slot;		// 更新槽位
	AbilityInfoDelegate.Broadcast(Info); // 广播更新后的能力信息
}


