// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UProjectileSpell
{
	GENERATED_BODY()
public:
	virtual FString GetDescription_Implementation(int32 Level) override;
	virtual FString GetNextLevelDescription_Implementation(int32 Level) override;
	/**
	 * 当前技能的技能描述，将共用部分抽离出来为单个函数
	 * @param Level 显示的技能等级
	 * @param Title 技能标题
	 * @return 技能描述文本
	 */
	virtual FString GetDescriptionAtLevel(int32 Level, const FString& Title) override; //获取对应等级的技能描述

	/**
	 * 生成多个火球函数，需要设置使用的骨骼
	 * 
	 * @param ProjectileTargetLocation 目标位置
	 * @param SocketTag 骨骼标签，用于判断骨骼位置是属于身体还是武器
	 * @param bOverridePitch 覆盖当前生成火球的垂直角度
	 * @param PitchOverride 覆盖的值
	 * @param HomingTarget 攻击的目标
	 *
	 */	
	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, const bool bOverridePitch = false, const float PitchOverride = 0.f, AActor* HomingTarget = nullptr);

protected:
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
