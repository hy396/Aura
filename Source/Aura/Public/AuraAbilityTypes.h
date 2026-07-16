// Huanyu,miaomiaomiao,nekoneko

#pragma once //预处理指令 确保这个头文件只被包含（include）一次，防止重复定义。

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}
	// 世界上下文对象
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	// 伤害GameplayEffect类
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	// 源Asc
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

	// 目标Asc
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	// 基础伤害
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;

	// 能力等级
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;

	// 伤害类型
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	// 负面效果几率
	UPROPERTY(BlueprintReadWrite)
	float DeBuffChance = 0.f;

	// 负面效果伤害
	UPROPERTY(BlueprintReadWrite)
	float DeBuffDamage = 0.f;

	// 负面效果持续时间
	UPROPERTY(BlueprintReadWrite)
	float DeBuffDuration = 0.f;

	// 负面效果频率
	UPROPERTY(BlueprintReadWrite)
	float DeBuffFrequency = 0.f;

	// 死亡冲击力大小
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;
	
	// 死亡时受到冲击的朝向
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;

	// 击退力大小
	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.f;

	// 击退几率
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;

	// 击退力
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;

	// 是否为范围伤害
	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;

	// 范围伤害内半径
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;

	// 范围伤害外半径
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;

	// 范围伤害起点
	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
};
USTRUCT(BlueprintType) //在蓝图中可作为类型使用
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY() //宏 自动生成构造函数、析构函数、拷贝构造函数等

public:
	// 是否为暴击
	bool IsCriticalHit() const { return bIsCriticalHit; }
	// 是否为格挡命中
	bool IsBlockedHit () const { return bIsBlockedHit; }
	// 是否为成功的负面效果
	bool IsSuccessfulDeBuff() const { return bIsSuccessfulDeBuff; }
	// 获取负面效果伤害
	float GetDeBuffDamage() const { return DeBuffDamage; }
	// 获取负面效果持续时间
	float GetDeBuffDuration() const { return DeBuffDuration; }
	// 获取负面效果频率
	float GetDeBuffFrequency() const { return DeBuffFrequency; }
	// 获取伤害类型
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	// 获取死亡时受到冲击的朝向
	FVector GetDeathImpulse() const { return DeathImpulse; }
	// 获取击退力
	FVector GetKnockbackForce() const { return KnockbackForce; }
	// 是否为范围伤害
	bool IsRadialDamage() const { return bIsRadialDamage; }
	// 获取范围伤害内半径
	float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius; }
	// 获取范围伤害外半径
	float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius; }
	// 获取范围伤害起点
	FVector GetRadialDamageOrigin() const { return RadialDamageOrigin; }
	
	// 设置是否为暴击
	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	// 设置是否为格挡命中
	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	// 设置是否为成功的负面效果
	void SetIsSuccessfulDeBuff(const bool bInIsDeBuff) { bIsSuccessfulDeBuff = bInIsDeBuff; }
	// 设置负面效果伤害
	void SetDeBuffDamage(const float InDamage) { DeBuffDamage = InDamage; }
	// 设置负面效果持续时间
	void SetDeBuffDuration(const float InDuration) { DeBuffDuration = InDuration; }
	// 设置负面效果频率
	void SetDeBuffFrequency(const float InFrequency) { DeBuffFrequency = InFrequency; }
	// 设置伤害类型
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; }
	// 设置死亡时受到冲击的朝向
	void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }
	// 设置击退力
	void SetKnockbackForce(const FVector& InForce) { KnockbackForce = InForce; }
	// 设置是否为范围伤害
	void SetIsRadialDamage(const bool bInIsRadialDamage) { bIsRadialDamage = bInIsRadialDamage; }
	// 设置范围伤害内半径
	void SetRadialDamageInnerRadius(const float InRadialDamageInnerRadius) { RadialDamageInnerRadius = InRadialDamageInnerRadius; }
	// 设置范围伤害外半径
	void SetRadialDamageOuterRadius(const float InRadialDamageOuterRadius) { RadialDamageOuterRadius = InRadialDamageOuterRadius; }
	// 设置范围伤害起点
	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) { RadialDamageOrigin = InRadialDamageOrigin; }
	
	/** 返回用于序列化的实际结构体 */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	/** 创建一个副本，用于后续网络复制或者后续修改 */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this; //WithCopy 设置为true，就可以通过赋值操作进行拷贝
		if (GetHitResult())
		{
			// 深拷贝 hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	/** 用于序列化类的参数 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;


protected:
	// 是否为格挡命中
	UPROPERTY()
	bool bIsBlockedHit = false;
	
	// 是否为暴击
	UPROPERTY()
	bool bIsCriticalHit = false;
	// 是否为成功的负面效果
	UPROPERTY()
	bool bIsSuccessfulDeBuff = false;

	// 负面效果伤害
	UPROPERTY()
	float DeBuffDamage = 0.f;

	// 负面效果持续时间
	UPROPERTY()
	float DeBuffDuration = 0.f;

	// 负面效果频率
	UPROPERTY()
	float DeBuffFrequency = 0.f;

	// 伤害类型
	TSharedPtr<FGameplayTag> DamageType;

	// 死亡时受到冲击的朝向
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	// 击退力
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;

	// 是否为范围伤害
	UPROPERTY()
	bool bIsRadialDamage = false;

	// 范围伤害内半径
	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;

	// 范围伤害外半径
	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;

	// 范围伤害起点
	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
};
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
