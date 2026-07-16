// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& InCooldownTag)
{
	// 创建一个异步任务,并对传入的参数对异步任务进行初始化
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	// 如果参数有一项为设置，我们将结束任务
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}
	// 监听冷却标签的变动，并绑定回调，用于获取冷却结束
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag,
		EGameplayTagEventType::NewOrRemoved).AddUObject(
			WaitCooldownChange,
			&UWaitCooldownChange::CooldownTagChanged);
	//监听GE应用回调，获取冷却激活，用于获取技能开始进入冷却
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);

	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	// 防御性编程：确保ASC未被提前销毁
	if (!IsValid(ASC)) return;
	// 取消对冷却标签的变动监听
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	// UE对象生命周期管理标准操作
	SetReadyToDestroy();	// 设置此对象可以被销毁，如果此对象不再被引用，将可以被销毁掉（触发OnDestroy逻辑）
	MarkAsGarbage();		// 加入垃圾回收队列（下一帧自动回收）
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	//如果计数为0，代表冷却标签已经不存在，技能不处于冷却状态
	if (NewCount == 0)
	{
		// 广播冷却结束委托
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// 获取设置到自身的所有标签
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	// 获取到GE设置给Actor的标签
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	// 判断应用的GE是否设置了此冷却标签
	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		// 创建一个查询对象，用于查询包含所有标签容器标签的GE
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
		// 返回查询到的所有包含此冷却GE的剩余时间的GE
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if (TimesRemaining.Num() > 0)
		{
			// 获取最高的冷却时间
			float TimeRemaining = TimesRemaining[0];
			for(int32 i=0; i<TimesRemaining.Num(); i++)
			{
				if(TimeRemaining < TimesRemaining[i])
				{
					TimeRemaining = TimesRemaining[i];
				}
			}
			// 广播冷却开始委托
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
