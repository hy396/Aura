// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

//这里结构体不加F是因为它是内部结构体，不需要外部获取，也不需要在蓝图中使用
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	AuraDamageStatics()
	{
		// 参数：1.属性集 2.属性名 3.目标还是自身 4.是否设置快照（true为创建时获取，false为应用时获取）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
		
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}
UExecCalc_Damage::UExecCalc_Damage()
{
	// 将属性添加到捕获列表中
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
	
}

void UExecCalc_Damage::DetermineDeBuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                       const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters,
                                       const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const
{
	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();

	// 遍历所有的负面效果伤害类型,根据伤害类型是否赋值来判断是否需要应用负面效果
	for (const TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDeBuffs)
	{
		// 获取到负面效果伤害类型
		FGameplayTag DamageType = Pair.Key;
		// 获取到负面效果抵抗类型
		//const FGameplayTag& DeBuffType = Pair.Value;
		const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
		// 获取当前伤害类型的伤害值
        const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
        if (TypeDamage > -0.5f) // 检查伤害值是否有效，允许一定的浮点误差
        {
            // 获取来源的减益成功率
            const float SourceDeBuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.DeBuff_Chance, false, -1.f);
        	// 初始化目标方的减益抗性值
            float TargetDeBuffResistance = 0.f;
        	// 根据抗性标签从目标属性中计算抗性值
            ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDefs[ResistanceTag], EvaluationParameters, TargetDeBuffResistance);
        	// 确保抗性值不为负数（防止异常数据）
        	TargetDeBuffResistance = FMath::Max<float>(TargetDeBuffResistance, 0.f); // 确保抗性值不为负

        	// 计算实际生效的减益成功率 = 攻击方成功率 × (100 - 目标抗性)/100
            const float EffectiveDeBuffChance = SourceDeBuffChance * (100 - TargetDeBuffResistance) / 100.f;

        	// 通过随机数判断是否成功触发减益（例如：如果有效概率是30%，则1-30之间的随机数会触发）
            const bool bDeBuff = FMath::RandRange(1, 100) < EffectiveDeBuffChance;
            if (bDeBuff)
            {
                // 如果减益成功，设置相关的上下文信息
                FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

                // 标记减益成功
                UAuraAbilitySystemLibrary::SetIsSuccessfulDeBuff(ContextHandle, true);
                
            	// 从伤害效果中获取减益的伤害值/持续时间/频率（-1表示未设置）
            	const float DeBuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.DeBuff_Damage, false, -1.f);
            	const float DeBuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.DeBuff_Duration, false, -1.f);
            	const float DeBuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.DeBuff_Frequency, false, -1.f);
            
            	// 将减益参数记录到上下文中，供后续应用减益效果使用
            	UAuraAbilitySystemLibrary::SetDeBuff(
					ContextHandle, 
					DamageType, 
					DeBuffDamage, 
					DeBuffDuration, 
					DeBuffFrequency
				);
            }
        }
		
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//存储标签和属性快照对应的Map
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FMyGameplayTags& Tags = FMyGameplayTags::Get();
		
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
	
	//获取ASC
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	//获取AvatarActor
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	// 获取挂载此类的GE实例
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	// 获取GE的上下文句柄
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();


	// 设置评估参数
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//获取到角色配置数据
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

	// DeBuff
	DetermineDeBuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);

	//从Set by Caller 获取Damage的伤害值
	float Damage = 0.f; //Spec.GetSetByCallerMagnitude(FMyGameplayTags::Get().Damage);
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FMyGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		// 检查对应的属性快照是否设置，防止报错
		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs 不包含 Tag: [%s] 在 ExecCalc_Damage 中"), *ResistanceTag.ToString());
		// 通过抗性标签获取到属性快照
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);
		if (DamageTypeValue <= 0.f)
		{
			continue;
		}
		//获取抗性值
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
		//通过抗性计算出能够对角色造成的伤害值
		DamageTypeValue *= ( 100.f - Resistance ) / 100.f;

		if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			// 1. 覆写 TakeDamage 函数，通过函数获取范围技能能够造成的最终伤害
			// 2. 创建一个委托 OnDamageDelegate， 在TakeDamage里向外广播最终伤害数值
			// 3. 在战斗接口声明一个函数用于返回委托，并在角色基类实现，在计算伤害时通过战斗接口获取到委托，并绑定匿名函数
			// 4. 调用 UGameplayStatics::ApplyRadialDamageWithFalloff 函数应用伤害，函数内会调用角色身上的TakeDamage来广播委托。
			// 5. 在匿名函数中，修改实际造成的伤害。

			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
			{
				CombatInterface->GetOnDamageSignature().AddLambda([&](float DamageAmount)
				{
					DamageTypeValue = DamageAmount;
				});
			}
			
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvatar,
				DamageTypeValue,
				0.f,
				UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatar,
				nullptr);
			// DamageTypeValue = UAuraAbilitySystemLibrary::ApplyRadialDamageWithFalloff(
			// 	TargetAvatar,
			// 	DamageTypeValue,
			// 	0.f,
			// 	UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
			// 	UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
			// 	UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
			// 	1.f,
			// 	SourceAvatar,
			// 	nullptr);
		}		
		
		//将每种属性伤害值合并进行后续计算
		Damage += DamageTypeValue;
	}
	
	//--------------------处理格挡路--------------------
	//获取格挡率，如果触发格挡，伤害减少一半
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max(0.f, TargetBlockChance);
	//根据格挡概率判断当前是否触发
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	// 设置格挡
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	if(bBlocked) Damage *= 0.5f;

	//--------------------处理目标护甲和源的护甲穿透影响伤害--------------------
	//获取目标护甲值
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max(0.f, TargetArmor);
	//获取源护甲穿透
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(0.f, SourceArmorPenetration);
	//获取到数据表内的护甲穿透系数
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	
	if (!ArmorPenetrationCurve)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, 
	 		FString::Printf(TEXT("ArmorPenetrationCurve: No!")));
		return;
	}
	
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
	//护甲穿透将按照比例忽略目标的护甲值，护甲穿透的值可以根据某个方程计算出实际护甲穿透率（可以根据等级，等级越高，每一点护甲穿透值的比例越低）
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	//获取到数据表内的护甲系数
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	if (!EffectiveArmorCurve)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, 
			 FString::Printf(TEXT("EffectiveArmorCurve: No!")));
		return;
	}
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	//忽略后的护甲值将以一定比例影响伤害
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	//--------------------暴击伤害--------------------
	//暴击率
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(0.f, SourceCriticalHitChance);
	//暴击伤害
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(0.f, SourceCriticalHitDamage);
	//暴击抵抗
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(0.f, TargetCriticalHitResistance);
	//获取到数据表内的暴击抵抗系数
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	if (!CriticalHitResistanceCurve)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, 
			 FString::Printf(TEXT("CriticalHitResistanceCurve: No!")));
		return;
	}
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);
	// 计算当前是否暴击
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
	// 设置暴击
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	//触发暴击 伤害乘以暴击伤害率
	if(bCriticalHit) Damage = Damage * 2.f + SourceCriticalHitDamage;


	
	//输出计算结果
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}


