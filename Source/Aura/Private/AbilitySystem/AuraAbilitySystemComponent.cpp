// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	/** 在服务器上，每当GE应用于自身时调用。这包括即时和持续时间的GE。 */
	//FOnGameplayEffectAppliedDelegate OnGameplayEffectAppliedDelegateToSelf;
	//OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied);
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
	
	// //测试
	// const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	// //GameplayTags.Attributes_Secondary_Armor.ToString() //标签的文本
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	10.f,
	// 	FColor::Blue,
	// 	FString::Printf(TEXT("Tag: %s"), *GameplayTags.Attributes_Secondary_Armor.ToString())
	// 	);
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveGameData)
{
	for (const FSavedAbility& Data : SaveGameData->SavedAbilities)
	{
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;

		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, Data.AbilityLevel);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityInputTag); //设置技能激活输入标签
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityStatus); //设置技能状态标签

		// 主动技能的处理
		if (Data.AbilityType == FMyGameplayTags::Get().Abilities_Type_Offensive)
		{
			// 给予技能(只应用不激活)
			GiveAbility(LoadedAbilitySpec);
		}else if (Data.AbilityType == FMyGameplayTags::Get().Abilities_Type_Passive)
		{
			// 被动技能的处理
			// 确保技能已经装配
			if(Data.AbilityStatus.MatchesTagExact(FMyGameplayTags::Get().Abilities_Status_Equipped))
			{
				GiveAbilityAndActivateOnce(LoadedAbilitySpec); //应用技能并激活一次
			}
			else
			{
				GiveAbility(LoadedAbilitySpec); //只应用不激活
			}
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for(const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FMyGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec); //只应用不激活
			//GiveAbilityAndActivateOnce(AbilitySpec); //应用技能并激活一次
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(FMyGameplayTags::Get().Abilities_Status_Equipped);
		// 应用技能并激活一次
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return; // 如果输入标签无效，则返回
	FScopedAbilityListLock ActiveScopeLoc(*this); // 创建一个Ability列表的锁
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) // 遍历所有可激活的Ability
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) // 如果Ability的动态标签与输入标签匹配
		{
			AbilitySpecInputPressed(AbilitySpec); // 调用AbilitySpecInputPressed函数
			if (AbilitySpec.IsActive()) // 如果Ability处于激活状态
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey()); // 调用InvokeReplicatedEvent函数，传递参数
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	// 遍历所有已经应用的技能，GetActivatableAbilities()将返回一个可激活技能的列表
	for (auto AbilitySpec : GetActivatableAbilities())
	{
		// 判断Spec上设置的Tag和传入的Tag是否相同
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//在技能悬停事件里面，我们首先调用告知GameplayAbility，此技能被触发按下事件，
			AbilitySpecInputPressed(AbilitySpec);
			// 防止技能重复被激活，判断当前技能是否处于激活状态
			if (!AbilitySpec.IsActive())
			{
				// 尝试激活技能
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	// 遍历所有已经应用的技能，GetActivatableAbilities()将返回一个可激活技能的列表
	for (auto AbilitySpec : GetActivatableAbilities())
	{
		// 判断Spec上设置的Tag和传入的Tag是否相同
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 告知技能按键事件被抬起
			// 释放AbilitySpec
			AbilitySpecInputReleased(AbilitySpec);
			// 触发客户端输入释放事件的服务器同步，使用预测键确保网络一致性
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased,
				AbilitySpec.Handle,
				AbilitySpec.ActivationInfo.GetActivationPredictionKey());

		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this); //使用域锁将此作用域this的内容锁定（无法修改），在遍历结束时解锁，保证线程安全
	for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(!Delegate.ExecuteIfBound(AbilitySpec)) //运行绑定在技能实例上的委托，如果失败返回false
		{
			UE_LOG(LogAura, Error, TEXT("在函数[%hs]运行委托失败"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags) //获取设置的所有的技能标签并遍历
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")))) //判断当前标签是否包含"Abilities"名称
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags) //从技能实例的动态标签容器中遍历所有标签
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag")))) //查找标签中是否设置以输入标签开头的标签
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)//从技能实例的动态标签容器中遍历所有标签
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))//查找标签中是否设置以输入标签开头的标签
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	// 线程安全：锁定技能列表防止遍历时被修改
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			return false;
		}
	}
	return true;
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	//通过判断动态标签是否含有Input的标签来判断技能是否装配到槽位
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	// 线程安全：锁定技能列表防止遍历时被修改
	FScopedAbilityListLock ActiveScopeLoc(*this);
	// 遍历所有可激活的技能
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	//从技能配置数据里获取到技能对于的配置信息
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	//判断信息里配置的技能类型是否为被动技能
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(FMyGameplayTags::Get().Abilities_Type_Passive);
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);
	Spec.DynamicAbilityTags.AddTag(Slot);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
	bool bActivate)
{
	ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	// 1. 通过技能标签查找对应的技能实例
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		// 2. 从技能实例中提取状态标签
		return GetStatusFromSpec(*Spec);
	}
	// 3. 未找到对应技能时返回空标签
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	// 1. 通过技能标签查找对应的技能实例
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		// 2. 从技能实例中提取输入标签
		return GetInputTagFromSpec(*Spec);
	}
	// 3. 未找到对应技能时返回空标签
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	// 线程安全：锁定技能列表防止遍历时被修改
	FScopedAbilityListLock ActiveScopeLoc(*this);
	// 遍历所有可激活的技能
	for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		// 检查技能的所有标签
		for (FGameplayTag Tag: AbilitySpec.Ability.Get()->AbilityTags)
		{
			// 使用层级匹配（例如"Ability.Fire"匹配"Ability.Fire.Basic"）
			if (Tag.MatchesTag(AbilityTag))
			{
				// 返回首个匹配的技能实例
				return &AbilitySpec;
			}
		}
	}
	// 未找到匹配项
	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	// 判断Avatar是否有角色接口
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		// 判断是否有可分配属性点
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload; //创建一个事件数据
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	
	// 向自身发送事件，通过被动技能接收属性加点
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	// 判断Avatar是否基础角色接口
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		// 减少一点可分配属性点
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	// 从GameMode获取到技能配置数据
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	// 遍历所有预配置的技能信息
	for (const FAuraAbilityInfo& Info: AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid())
		{
			//如果没有技能标签，取消执行
			continue;
		}
		if (Level < Info.LevelRequirement)
		{
			//如果当期等级未达到所需等级，取消执行
			continue;
		}
		// 判断ASC中是否已存在当前技能实例
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			// 如果没有技能实例，将应用一个新的技能实例
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability,1);
			// 设置技能初始状态标签
			AbilitySpec.DynamicAbilityTags.AddTag(FMyGameplayTags::Get().Abilities_Status_Eligible);
			// 只应用不激活
			GiveAbility(AbilitySpec);
			// 标记需要立即同步到客户端（网络游戏关键步骤）
			MarkAbilitySpecDirty(AbilitySpec); //设置当前技能立即复制到每个客户端
			ClientUpdateAbilityStatus(Info.AbilityTag,FMyGameplayTags::Get().Abilities_Status_Eligible, 1);

		}
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Slot)
{
	// 通过技能标签寻找技能实例
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		// 获取游戏标签单列
		const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
		// 技能之前装配的插槽
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		// 当前技能的状态标签
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		// 判断技能的状态，技能的状态只有在已装配或者已解锁的状态才可以装配
		if (Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked)
		{
			// 判断插槽是否有技能，有则需要将其清除
			if (!SlotIsEmpty(Slot))
			{
				// 获取目标插槽现在装配的技能
				if (const FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot))
				{
					//技能槽位装配相同的技能，直接返回，不做额外的处理
					if(AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, Status, Slot, PrevSlot);
						return;
					}
					
					//如果是被动技能，我们需要先将技能取消执行
					if(IsPassiveAbility(*SpecWithSlot))
					{
						// 关闭特效
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
						DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}
					
					ClearAbilitiesOfSlot(Slot); //清除目标插槽装配的技能
				}
			}
			//技能没有设置到插槽（没有激活）
			if (!AbilityHasAnySlot(*AbilitySpec)) // Ability doesn't yet have a slot (it's not active)
			{
				//如果是被动技能，装配即激活
				if (IsPassiveAbility(*AbilitySpec))
				{
					// 激活技能
					TryActivateAbility(AbilitySpec->Handle);

					// 激活特效
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
				AbilitySpec->DynamicAbilityTags.RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			// 修改技能装配的插槽
			AssignSlotToAbility(*AbilitySpec, Slot);

			// 立即将其复制到每个客户端
			MarkAbilitySpecDirty(*AbilitySpec);
			
			//回调更新UI
			ClientEquipAbility(AbilityTag, Status, Slot, PrevSlot);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	// 在客户端将更新后的标签广播
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	// 获取技能输入标签
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	// 移除技能输入标签
	Spec->DynamicAbilityTags.RemoveTag(Slot);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, Slot))
		{
			ClearSlot(&Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for(FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if(Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	// 获取到技能实例
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		//减少一个可分配的技能点
		if(GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		// 获取状态标签
		const FMyGameplayTags GameplayTags = FMyGameplayTags::Get();
		FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);
		
		// 根据状态标签处理
		if (StatusTag.MatchesTag(GameplayTags.Abilities_Status_Eligible))
		{
			// 技能升级，如果是可解锁状态，将状态标签从可解锁，切换为已解锁
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			StatusTag = GameplayTags.Abilities_Status_Unlocked;
			// 等待解锁阶段,默认等级是1级，解锁完让他是一级
			AbilitySpec->Level = 1;
		}else if(StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			// 技能升级，如果是已装配或者解锁状态，将等级加1
			AbilitySpec->Level += 1;
		}
		// 广播技能状态修改
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);
		// 设置当前技能立即复制到每个客户端
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                              FString& OutNextLevelDescription)
{
	// 如果当前技能处于锁定状态，将无法获取到对应的技能描述
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if(UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	// 如果技能是锁定状态，将显示锁定技能描述
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FMyGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) const
{
	//客户端函数的实现需要加上_Implementation后缀，这样就实现了在客户端调用。
	//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Effect Applied")));
	FGameplayTagContainer TagContainer;
	// 获取所有资产标签
	EffectSpec.GetAllAssetTags(TagContainer);
	// 广播EffectAssetTags，参数为TagContainer
	EffectAssetTags.Broadcast(TagContainer);
}
