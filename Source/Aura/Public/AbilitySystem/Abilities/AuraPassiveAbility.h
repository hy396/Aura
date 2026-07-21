// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraPassiveAbility.generated.h"

/**
 * 被动技能基类。被动技能完全在蓝图（GA_HaloOfProtection / GA_LifeSiphon / GA_ManaSiphon 等）中实现，
 * 其技能描述请在蓝图事件图中覆写 GetDescription(Level) 与 GetNextLevelDescription(Level) 两个事件，
 * 直接返回富文本字符串即可（基类已将这两个函数声明为 BlueprintNativeEvent）。
 */
UCLASS()
class AURA_API UAuraPassiveAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	/**
	 * 覆写激活技能函数
	 * @param Handle 技能实力的句柄
	 * @param ActorInfo 技能拥有者
	 * @param ActivationInfo 激活信息
	 * @param TriggerEventData 游戏事件信息
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * 接收到技能结束回调函数
	 * @param AbilityTag 结束的技能标识标签
	 */
	void ReceiveDeactivate(const FGameplayTag& AbilityTag);

};
