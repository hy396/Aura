// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	//生成配置
	FDamageEffectParams Params = MakeDamageEffectParamsFromClassDefaults(TargetActor);

	//设置死亡冲击和击退
	if(IsValid(TargetActor))
	{
		//获取到攻击对象和目标的朝向，并转换成角度
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f; //设置击退角度垂直45度
		const FVector ToTarget = Rotation.Vector();
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		//判断攻击是否触发击退
		if(FMath::RandRange(1, 100) < Params.KnockbackChance)
		{
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
		}
	}
		
	//通过配置项应用给目标ASC
	UAuraAbilitySystemLibrary::ApplyDamageEffect(Params);
                        
	// // 创建GE
	// FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	// // 通过SetByCaller设置属性伤害
	// const float ScaleDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaleDamage);
	// //将GE应用给目标
	// GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
	// 	*DamageSpecHandle.Data.Get(),
	// 	UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector KnockbackDirectionOverride,
	bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	FDamageEffectParams Params;
	// === 基础参数配置 ===
    Params.WorldContextObject = GetAvatarActorFromActorInfo(); // 设置世界上下文对象（施法者Actor）
    Params.DamageGameplayEffectClass = DamageEffectClass;     // 设置伤害效果类
    Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo(); // 设置来源ASC组件
    Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor); // 获取目标ASC组件
    Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel()); // 根据等级计算基础伤害值
    Params.AbilityLevel = GetAbilityLevel(); // 设置能力等级
    Params.DamageType = DamageType;          // 设置伤害类型标签

    // === 负面效果参数 ===
    Params.DeBuffChance = DeBuffChance;      // 触发负面效果的概率（百分比）
    Params.DeBuffDamage = DeBuffDamage;      // 负面效果造成的伤害值
    Params.DeBuffDuration = DeBuffDuration;  // 负面效果持续时间（秒）
    Params.DeBuffFrequency = DeBuffFrequency;// 负面效果触发间隔时间（秒）

    // === 击退与死亡冲击参数 ===
    Params.DeathImpulseMagnitude = DeathImpulseMagnitude; // 死亡冲击力的强度
    Params.KnockbackForceMagnitude = KnockbackForceMagnitude; // 击退力的强度
    Params.KnockbackChance = KnockbackChance; // 触发击退的概率（百分比）

    // === 方向计算与覆盖逻辑 ===
    if (IsValid(TargetActor))
    {
        // 计算施法者到目标的方向向量
        FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
        
        // 处理Pitch覆盖
        if (bOverridePitch)
        {
            Rotation.Pitch = PitchOverride; // 设置自定义俯仰角
        }
        
        const FVector ToTarget = Rotation.Vector(); // 获取标准化方向向量
        
        // 若未覆盖方向，则使用默认方向（施法者到目标）
        if (!bOverrideKnockbackDirection)
        {
            Params.KnockbackForce = ToTarget * KnockbackForceMagnitude; // 计算击退力
        }
        
        if (!bOverrideDeathImpulse)
        {
            Params.DeathImpulse = ToTarget * DeathImpulseMagnitude; // 计算死亡冲击力
        }
    }

    // === 自定义方向覆盖逻辑 ===
    if (bOverrideKnockbackDirection)
    {
        KnockbackDirectionOverride.Normalize(); // 标准化自定义方向
        Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude; // 应用自定义方向
        
        // 若同时覆盖Pitch，则重新计算方向
        if (bOverridePitch)
        {
            FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
            KnockbackRotation.Pitch = PitchOverride;
            Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
        }
    }

    if (bOverrideDeathImpulse)
    {
        DeathImpulseDirectionOverride.Normalize(); // 标准化自定义方向
        Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude; // 应用自定义方向
        
        // 若同时覆盖Pitch，则重新计算方向
        if (bOverridePitch)
        {
            FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
            DeathImpulseRotation.Pitch = PitchOverride;
            Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
        }
    }

    // === 范围伤害参数配置 ===
    if (bIsRadialDamage)
    {
        Params.bIsRadialDamage = true; // 标记为范围伤害
        Params.RadialDamageOrigin = InRadialDamageOrigin; // 设置范围伤害原点坐标
        Params.RadialDamageInnerRadius = RadialDamageInnerRadius; // 内半径（内圈全额伤害）
        Params.RadialDamageOuterRadius = RadialDamageOuterRadius; // 外半径（外圈无伤害）
    }

	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}

	return FTaggedMontage();
}

