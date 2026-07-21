// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;

	/**
	 * 获取当前等级技能描述（蓝图可覆写，被动技能在蓝图里编写描述）
	 * @param Level 技能等级
	 * @return 技能描述富文本
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FString GetDescription(int32 Level);

	/**
	 * 获取下一等级技能描述（蓝图可覆写，被动技能在蓝图里编写描述）
	 * @param Level 下一等级
	 * @return 技能描述附文本
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FString GetNextLevelDescription(int32 Level);
	/**
	 * 当前技能的技能描述，将共用部分抽离出来为单个函数
	 * @param Level 显示的技能等级
	 * @param Title 技能标题
	 * @return 技能描述文本
	 */
	virtual FString GetDescriptionAtLevel(int32 Level, const FString& Title);

	// 获取锁定技能描述
	static FString GetLockedDescription(int32 Level);

protected:

	float GetManaCost(float InLevel = 1.f) const;
	float GetCooldown(float InLevel = 1.f) const;

};
