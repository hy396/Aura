// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UAuraAttributeSet::UAuraAttributeSet()
{

	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	// FAttributeSignature StrengthDelegate;
	// // 绑定静态函数GetStrengthAttribute到StrengthDelegate
	// StrengthDelegate.BindStatic(GetStrengthAttribute);
	// TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, StrengthDelegate);
	
	/* 主要属性(Primary Attributes) */
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	
	/* 次要属性(Secondary Attributes) */
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);	
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	
	/* 抗性属性(Resistance Attributes) */
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
	// InitHealth(66.f);
	// InitMana(99.f);
	//使用MMC在GE初始化定义数值
	//InitMaxHealth(333.f);
	//InitMaxMana(888.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 主要属性(Primary Attributes)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	// 次要属性(Secondary Attributes)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// 抗性属性(Resistance Attributes)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	
	// 关键属性(Vital Attributes)
	// 当Health属性发生变化时，始终通知
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	//修改返回值
	// TODO:感觉没什么用的样子 但人机说👉 实际非常必要
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}

}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
                                            FEffectProperties& Props) const
{
	//Source 效果的所有者   Target 效果应用的目标
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	//获取效果所有者的相关对象
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceAvatarActor != nullptr && Props.SourceController == nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->Controller;
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	/**
	 * 死亡状态提前返回检查
	 * 
	 * 功能说明：
	 * 1. 接口有效性验证 - 确保目标角色实现了战斗接口
	 * 2. 死亡状态检测 - 通过战斗接口查询角色是否已死亡
	 * 3. 逻辑短路 - 若角色已死亡则直接退出伤害处理流程
	 * 
	 * 设计要点：
	 * - 双重检查机制：先验证接口实现再执行查询，避免空指针崩溃
	 * - 状态优先原则：已死亡角色不再响应后续伤害逻辑（受击反应/伤害数字等）
	 * - 性能优化：减少对死亡角色的无效计算
	 * 
	 * 典型应用场景：
	 * - 处理持续伤害效果时（如燃烧DOT）
	 * - 处理多段攻击的后续伤害时
	 * - 网络同步场景下的延迟伤害处理
	 */
	if(Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter))
	{
		return; // 目标已死亡，跳过后续伤害处理
	}

	// 限制值的范围
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("%s 的生命值发生了修改，当前生命值：%f"), *Props.TargetAvatarActor->GetName(), GetHealth());
	}

	if(Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	// 造成伤害
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	// 获取经验值
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	// if (!FMath::IsNearlyEqual(OldValue, NewValue)) //判断值是否产生了变化
	// {
	// 	if (Attribute == GetMaxHealthAttribute())
	// 	{
	// 		const float CurrentValue = GetHealth(); //获取当前的实际值
	// 		float NewDelta = (OldValue > 0.f) ? (CurrentValue * NewValue / OldValue) - CurrentValue : NewValue; //获取到最大值变动后,按比列修改后的值
	// 		// 确保新值大于等于0,防止被扣血
	// 		NewDelta = FMath::Max(NewDelta, 0.f);
	// 		SetHealth(NewDelta + CurrentValue);
	// 	}
	// 	else if (Attribute == GetMaxManaAttribute())
	// 	{
	// 		const float CurrentValue = GetMana(); //获取当前的实际值
	// 		float NewDelta = (OldValue > 0.f) ? (CurrentValue * NewValue / OldValue) - CurrentValue : NewValue; //获取到最大值变动后,按比列修改后的值
	// 		// 确保新值大于等于0
	// 		NewDelta = FMath::Max(NewDelta, 0.f);
	// 		SetMana(NewDelta + CurrentValue);
	// 	}
	// }
	if(Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}

	if(Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		const bool bFatal = NewHealth <= 0.f; //血量小于等于0时，角色将会死亡
		if (bFatal)
		{
			// 调用死亡函数
			if (Props.TargetAvatarActor->Implements<UCombatInterface>())
			{
				ICombatInterface::Execute_Die(Props.TargetAvatarActor,UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
			// 死亡时,发送经验事件
			SendXPEvent(Props);
		}else
		{
			//在受到闪电链攻击时，不会激活受击
			if(Props.TargetCharacter->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsBeingShocked(Props.TargetCharacter))
			{
				//激活受击技能
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FMyGameplayTags::Get().Effects_HitReact);
				// Props.TargetASC->CancelAbilities(&TagContainer); //先取消之前的受击
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer); //根据tag标签激活技能
			}
			// 判断当前是否应用负面效果
			if (UAuraAbilitySystemLibrary::IsSuccessfulDeBuff(Props.EffectContextHandle))
			{
				DeBuff(Props);
			}
			// 设置攻击击退效果
			const FVector& KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			// 如果击退的值不接近于0，则触发击退效果
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				//LaunchCharacter 是 ACharacter 类的成员函数，用于向角色施加一个瞬时的物理力（如击退、爆炸冲击等）。
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		}
		//获取格挡和暴击
		const bool IsBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool IsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		// 显示伤害数字
		ShowFloatingText(Props,LocalIncomingDamage, IsBlockedHit, IsCriticalHit,false);
	}
}
void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	//UE_LOG(LogAura, Warning, TEXT("%s 收到了经验值：%f"), *Props.TargetAvatarActor->GetName(), GetIncomingXP());
	SetIncomingXP(0.f);
	//ShowFloatingText(Props, LocalIncomingXP, false, false,true);
	//将经验应用给自身
	//UE_LOG(LogAura, Warning, TEXT("%s 收到了经验值：%f"), *Props.TargetAvatarActor->GetName(), GetIncomingXP());
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		// 获取角色当前等级和经验
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		//获取获得经验后的新等级
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumLevelUps = NewLevel - CurrentLevel; //查看等级是否有变化

		if (NumLevelUps > 0)
		{
			// 增加等级
			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
			// 获取升级提供的技能点和属性点
			int32 AttributePointsReward = 0;
			int32 SpellPointsReward = 0;
			for (int32 i = 0; i < NumLevelUps; ++i)
			{
				AttributePointsReward += IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel + i);
				SpellPointsReward += IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel + i);
			}
			// 增加技能点和属性点
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);
			// 触发等级提升事件
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
			//将血量和蓝量填充满
			// SetHealth(GetMaxHealth());
			// SetMana(GetMana());
			bTopOffMana = true;
			bTopOffHealth = true;
			
		}
		// 将经验应用给自身，通过事件传递，在玩家角色被动技能GA_ListenForEvents里接收
		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UAuraAttributeSet::DeBuff(const FEffectProperties& Props)
{
	// 获取游戏标签
	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	
	// 从效果上下文中获取伤害类型、减益伤害、持续时间和频率
	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DeBuffDamage = UAuraAbilitySystemLibrary::GetDeBuffDamage(Props.EffectContextHandle);
	const float DeBuffDuration = UAuraAbilitySystemLibrary::GetDeBuffDuration(Props.EffectContextHandle);
	const float DeBuffFrequency = UAuraAbilitySystemLibrary::GetDeBuffFrequency(Props.EffectContextHandle);

	// 动态生成减益效果的名称
	FString DeBuffName = FString::Printf(TEXT("DynamicDeBuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DeBuffName));

	// 设置减益效果持续类型为固定时长模式，效果将在指定时间后自动移除
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;

	// 设置效果触发间隔时间（单位：秒），控制减益效果的周期性应用频率
	Effect->Period = FScalableFloat(DeBuffFrequency);

	// 设置减益效果的总持续时长（单位：秒），决定效果生效的总时间
	Effect->DurationMagnitude = FScalableFloat(DeBuffDuration);
	
	// 在应用后不会立即触发，而是在经过了Period后才会触发
	Effect->bExecutePeriodicEffectOnApplication = false;
	
	// 设置每次应用后不会重置触发时间
	Effect->PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionRemovedPolicy::NeverReset; 


	//设置可叠加层数
	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource; //设置GE应用基于释放者查看
	Effect->StackLimitCount = 1; //设置叠加层数
	Effect->StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication; //在应用后重置时，重置持续时间
	Effect->StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication; //在应用时，触发并重置Period时间
	Effect->StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack; //GE时间到了默认清除所有层数，还有可以清除单层的设置
	//Effect->OverflowEffects.Add() //在叠加层数超出时，将触发此数组内的GE应用到角色
	// Effect->bDenyOverflowApplication = true; //设置为true时，叠加层数超出时，将不会刷新GE实例
	// Effect->bClearStackOnOverflow = true; //设置为true时，叠加层数超出时，将清除GE

                        
	//在5.3版本修改为通过GEComponent来设置GE应用的标签，向目标Actor增加对应的标签
	const FGameplayTag DeBuffTag = GameplayTags.DamageTypesToDeBuffs[DamageType];
	/** 弃用InheritableOwnedTagsContainer
	 * 	Effect->InheritableOwnedTagsContainer.AddTag(DeBuffTag);
	 */
	UTargetTagsGameplayEffectComponent& TargetTagsGameplayEffectComponent = Effect->AddComponent<UTargetTagsGameplayEffectComponent>();
	// 获取到标签容器
	//FInheritedTagContainer InheritableOwnedTagsContainer = TargetTagsGameplayEffectComponent.GetConfiguredTargetTagChanges();
	// 不应该直接获取，而是直接创造一个新的FInheritedTagContainer
	FInheritedTagContainer InheritableOwnedTagsContainer; //创建组件所需的标签容器
	// 添加标签
	InheritableOwnedTagsContainer.Added.AddTag(DeBuffTag); 
	// 如果被击晕了
	if (DeBuffTag.MatchesTagExact(FMyGameplayTags::Get().DeBuff_Stun))
	{
		// 添加标签
		InheritableOwnedTagsContainer.Added.AddTag(GameplayTags.Player_Block_CursorTrace); 
		InheritableOwnedTagsContainer.Added.AddTag(GameplayTags.Player_Block_InputHold); 
		InheritableOwnedTagsContainer.Added.AddTag(GameplayTags.Player_Block_InputPressed); 
		InheritableOwnedTagsContainer.Added.AddTag(GameplayTags.Player_Block_InputReleased);
	}
	// 应用并更新
	TargetTagsGameplayEffectComponent.SetAndApplyTargetTagChanges(InheritableOwnedTagsContainer);


	
	// 设置属性修改
	const int32 Index = Effect->Modifiers.Num(); //获取当前修改属性的Modifiers的长度，也就是下一个添加的modify的下标索引
	Effect->Modifiers.Add(FGameplayModifierInfo()); //添加一个新的Modify
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index]; //通过下标索引获取Modify

	// 设置应用的属性值
	ModifierInfo.ModifierMagnitude = FScalableFloat(DeBuffDamage); 
	// 设置属性运算符号
	ModifierInfo.ModifierOp = EGameplayModOp::Additive; 
	// 设置修改的属性
	ModifierInfo.Attribute = UAuraAttributeSet::GetIncomingDamageAttribute();

	// 创建GE实例，并添加伤害类型标签，应用GE
	FGameplayEffectContextHandle EffectContextHandle = Props.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);
	// 创建一个可变的效果规格，并将其应用到目标
	if (const FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContextHandle, 1.f))
	{
		// 设置效果上下文中的伤害类型
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());
		const TSharedPtr<FGameplayTag> DeBuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(DeBuffDamageType);

		// 将效果应用到目标
		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}                
	
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		// 从战斗接口获取目标角色等级和职业类型
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);//c++内调用BlueprintNativeEvent函数需要这样调用
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		// 根据职业和等级计算经验值奖励
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);
		
		const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
		// 配置游戏事件数据
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP; // 使用元属性标签标识经验值事件
		Payload.EventMagnitude = XPReward; // 设置经验值奖励数值

		// 显示经验
		ShowFloatingText(Props, Payload.EventMagnitude, false, false,true);

		// 向目标角色发送游戏事件（经验值获取事件）
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}
void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsBlockedHit,
	bool IsCriticalHit, bool bIsExp)
{
	
	for (int32 i = 0; ;++i)
	{
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(UGameplayStatics::GetPlayerController(Props.TargetCharacter,i)))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit, bIsExp); //调用显示伤害数字
		}else
		{
			break;
		}
	}
	// 下面方案因为无法创建火堆造成的伤害显示问题
	
	// 调用显示伤害数字
	// if (Props.SourceCharacter != Props.TargetCharacter)
	// {
	// 	// if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
	// 	// {
	// 	// 	PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit); //调用显示伤害数字				
	// 	// }
	// 	// 独家秘方， 每个角色都能显示伤害数字
	// 	for (int32 i = 0; ;++i)
	// 	{
	// 		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(UGameplayStatics::GetPlayerController(Props.TargetCharacter,i)))
	// 		{
	// 			PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit); //调用显示伤害数字
	// 		}else
	// 		{
	// 			break;
	// 		}
	// 	}
	// 	//从技能释放者身上获取PC并显示伤害数字
	// 	// if(AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
	// 	// {
	// 	// 	PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit); //调用显示伤害数字
	// 	// }
	// 	// //从目标身上获取PC并显示伤害数字
	// 	// if(AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))
	// 	// {
	// 	// 	PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit); //调用显示伤害数字
	// 	// }
	// }
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// 通知游戏属性集，生命值发生变化
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}


void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	// 通知游戏属性集法力值的改变
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	// 通知游戏属性集，力量值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	// 通知游戏属性集，智力值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	// 通知游戏属性集，韧性值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	// 通知游戏属性集，刚度值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	// 通知游戏属性集，护甲值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	// 通知游戏属性集，护甲穿透值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	// 通知游戏属性集，格挡机会值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	// 通知游戏属性集，暴击率已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	// 通知游戏属性集，暴击伤害值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	// 通知游戏属性集，暴击伤害抗性值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	//血量自动恢复
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	//法力自动恢复
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	// 通知游戏属性集，最大生命值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
	//UE_LOG(LogTemp, Warning, TEXT("[Client] OnRep_MaxHealth：%f"), MaxHealth.GetCurrentValue());
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	// 通知游戏属性集，最大法力值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	// 通知游戏属性集，火焰抗性值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	// 通知游戏属性集，雷电抗性值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	// 通知游戏属性集，魔法抗性值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	// 通知游戏属性集，物理抗性值已更改
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}
