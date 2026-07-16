// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Actor/AuraProjectile.h"
#include "ProjectileBarrage.generated.h"

/**
 * 定义投射物的基础属性配置，包含：
 * - 投射物类型
 * - 伤害效果
 * - 伤害类型与数值的映射关系
 */
USTRUCT(BlueprintType)
struct FProjectileType
{
	GENERATED_BODY()
	// 投射物类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
	// 伤害效果
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) //只能在蓝图中设置，可以在蓝图中读写
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	// 伤害类型映射
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
	// 负面效果伤害类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage",
		meta=(DisplayName="负面效果伤害类型"))
	FGameplayTag DeBuffDamageType = FGameplayTag();

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
	float KnockbackChance = 0.f;

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
/**
 * 
 */
UCLASS()
class AURA_API UProjectileBarrage : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	// 获取投射技能描述
	virtual FString GetDescription(int32 Level) override;
	// 获取投射技能下一等级描述
	virtual FString GetNextLevelDescription(int32 Level) override;
	FString GetDescriptionAtLevel(int32 Level, const FString& Title); //获取对应等级的技能描述

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
	/**
	 * 生成多个火球函数，需要设置使用的骨骼
	 * 
	 * @param InLocation 生成的位置
	 * @param ProjectileTargetLocation 目标位置
	 * @param bOverridePitch 覆盖当前生成火球的垂直角度
	 * @param PitchOverride 覆盖的值
	 * @param HomingTarget 攻击的目标
	 *
	 */	
	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectiles(const FVector& InLocation, const FVector& ProjectileTargetLocation, const bool bOverridePitch = false, const float PitchOverride = 0.f, AActor* HomingTarget = nullptr);

	// 投射物配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FProjectileType> ProjectileData;
	
	UPROPERTY(EditDefaultsOnly, Category="Projectile", 
		meta=(Tooltip="召唤魔法飞弹的数量"))
	int32 NumMinions = 8;
	
	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
		meta=(Tooltip="魔法飞弹距离魔法师最近的距离"))
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
		meta=(Tooltip="魔法飞弹距离魔法师最远的距离"))
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
		meta=(Tooltip="魔法飞弹在魔法师前面的角度范围"))
	float SpawnSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
	meta=(Tooltip="原点的高度"))
	float LocationHeight = 150.f;
	
	
	// 获取发射位置
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();
	
	UFUNCTION(BlueprintCallable, Category="Projectile", 
		meta=(Tooltip="生成发射物品.瞄准传入的目标位置,通过传入的骨骼标签来寻找发射位置"))
	void SpawnProjectile(const FVector& InLocation, const FVector& ProjectileTargetLocation, const bool bOverridePitch = false,const float PitchOverride = 0.f);
	UFUNCTION(BlueprintPure, Category="Projectile",
		meta=(Tooltip="获取随机魔法飞弹"))
	FORCEINLINE int32 GetRandomProjectile() const {return FMath::RandRange(0, ProjectileData.Num() - 1);};
	// UFUNCTION(BlueprintPure, Category="Projectile",
	// 	meta=(Tooltip="获取正态分布的位置返回"))
	// int32 SetRandomValue(const int32& InValue);
	// 最大等级
	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;
	UPROPERTY(BlueprintReadWrite)
	int32 Index = 0;


	// 技能攻击范围角度
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float ProjectileSpread = 90.f;

	// 技能攻击范围角度
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	int32 MaxNumProjectiles = 5;

	// 移动朝向目标的最小加速度
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMin = 1600.f;

	// 移动朝向目标的最大加速度
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMax = 3200.f;

	// 设置生成的飞弹是否需要朝向目标飞行
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	bool bLaunchHomingProjectiles = true;
};

