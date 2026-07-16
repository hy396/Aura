// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	/**
	 * 创建技能负面效果使用的结构体
	 * @param TargetActor 攻击的目标Actor（可选），用于计算方向和获取AbilitySystemComponent
	 * @param InRadialDamageOrigin 范围伤害的圆心位置（当bIsRadialDamage为true时生效）
	 * @param bOverrideKnockbackDirection 是否覆盖默认的击退方向（默认使用目标方向）
	 * @param KnockbackDirectionOverride 自定义击退方向向量（需为标准化向量）
	 * @param bOverrideDeathImpulse 是否覆盖默认的死亡冲击方向（默认使用目标方向）
	 * @param DeathImpulseDirectionOverride 自定义死亡冲击方向向量（需为标准化向量）
	 * @param bOverridePitch 是否覆盖默认的Pitch角度（影响击退/冲击方向的垂直角度）
	 * @param PitchOverride 自定义Pitch角度（以度为单位，0为水平方向）
	 * @return 配置好的伤害效果参数结构体FDamageEffectParams
	 * 
	 * @note 函数会自动处理向量标准化和方向计算，若传入非标准化向量会自动标准化
	 * @note 当bIsRadialDamage为true时，TargetActor参数将被忽略，使用InRadialDamageOrigin作为伤害中心
	 * @warning 传入的KnockbackDirectionOverride和DeathImpulseDirectionOverride必须为有效向量，零向量会导致方向异常
	 */
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector KnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;

protected:
	// 伤害效果类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) //只能在蓝图中设置，可以在蓝图中读写
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FScalableFloat Damage;
	//从角色设置的蒙太奇数组总，随机一个蒙太奇使用
	UFUNCTION(BlueprintPure , meta = (Keywords = "随机蒙太奇"))
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

	// TODO: 作者的做法似乎对此遗忘了
	// 负面效果伤害类型
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage",
	// 	meta=(DisplayName="负面效果伤害类型"))
	// FGameplayTag DeBuffDamageType = FGameplayTag();

	// 触发负面效果的概率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage",
		meta=(DisplayName="触发负面效果的概率"))
	float DeBuffChance = 20.f; 

	// 负面效果的伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage",
		meta=(DisplayName="负面效果的伤害"))
	float DeBuffDamage = 5.f; 

	// 负面伤害触发间隔时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage",
		meta=(DisplayName="负面伤害触发间隔时间"))
	float DeBuffFrequency = 1.f;

	// 负面效果持续时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage",
		meta=(DisplayName="负面效果持续时间"))
	float DeBuffDuration = 5.f;

	// 死亡冲击力的大小，用于角色死亡时的冲击效果
	UPROPERTY(EditDefaultsOnly, Category = "Damage",
		meta = (DisplayName = "死亡冲击力大小"))
	float DeathImpulseMagnitude = 1000.f;

	// 击退力的大小，当命中敌人时施加的击退力
	UPROPERTY(EditDefaultsOnly, Category = "Damage",
		meta = (DisplayName = "击退力"))
	float KnockbackForceMagnitude = 1000.f;

	// 击退触发的概率（百分比），例如20表示20%概率触发击退
	UPROPERTY(EditDefaultsOnly, Category = "Damage",
		meta = (DisplayName = "击退触发概率"))
	float KnockbackChance = 50.f;

	// 是否为范围伤害（True=范围伤害，False=单体伤害）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage",
		meta = (DisplayName = "是否为范围伤害"))
	bool bIsRadialDamage = false;

	// 范围伤害的内半径（在此半径内造成全额伤害）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage",
		meta = (DisplayName = "范围伤害内半径"))
	float RadialDamageInnerRadius = 0.f;

	// 范围伤害的外半径（超出此半径则不造成伤害）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage",
		meta = (DisplayName = "范围伤害外半径"))
	float RadialDamageOuterRadius = 0.f;
};
