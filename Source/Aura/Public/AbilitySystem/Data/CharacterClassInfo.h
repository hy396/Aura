// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "CharacterClassInfo.generated.h"

//角色职业类型的枚举
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	//法师
	Elementalist,
	//战士
	Warrior,
	//游侠
	Ranger 
};
/*----------------------------------------------
| FCharacterClassDefaultInfo 角色职业初始配置
| 包含职业专属的初始属性效果和技能能力配置
----------------------------------------------*/
USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()
	// 该职业初始拥有的核心属性效果（力量/智力等）
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
	
	// 职业初始自带的技能能力列表
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	// 击败后可获得的经验值
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
};

/*===============================================
| UCharacterClassInfo 角色职业配置数据资产
| 
| 核心功能：
| - 通过职业类型映射获取对应初始配置
| - 存储全职业通用属性/技能配置
| - 提供伤害计算所需曲线数据
===============================================*/
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	// 职业配置映射表（键：ECharacterClass枚举，值：包含主属性和初始技能）
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	// 主要属性，玩家的基础属性，通过SetByCaller设置
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes_SetByCaller;

	// 次级属性效果（护甲/暴击等）
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	// 生命属性效果（血量/法力等）
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;
	// 伤害计算系数（通过曲线控制不同等级的伤害公式）
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	/**
	 * 获取指定职业的初始配置数据
	 * @param CharacterClass - 要查询的职业类型枚举
	 * @return 包含主属性和初始技能的结构体
	 */
	FCharacterClassDefaultInfo GetClassDefaultInfo(const ECharacterClass CharacterClass) const;
	
};
