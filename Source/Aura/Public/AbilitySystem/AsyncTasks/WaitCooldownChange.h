// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ActiveGameplayEffectHandle.h"
#include "WaitCooldownChange.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask")) // 暴露异步代理供蓝图使用
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	// 冷却触发开始委托
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;

	// 冷却触发结束委托
	UPROPERTY(BlueprintAssignable)
  	FCooldownChangeSignature CooldownEnd;
  	// 创建异步任务并绑定到指定技能系统组件和冷却标签
	/**
	 * 异步任务创建接口
	 * 
	 * 功能：创建冷却监听任务实例并绑定到指定能力系统组件
	 * 
	 * @param AbilitySystemComponent - 要监听的能力系统组件（通常来自玩家角色）
	 * @param InCooldownTag - 需要监听的冷却效果标签（例：Cooldown.FireBlast）
	 * @return 配置好的异步任务实例指针
	 */
  	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true",	// 隐藏为纯函数节点
  		ToolTip="创建冷却时间监听器\n参数要求：\n- 有效的AbilitySystemComponent\n- Cooldown类型的GameplayTag"))
  	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag);
	/**
	 * 安全终止异步任务
	 * 
	 * 执行以下操作：
	 * 1. 有效性检查 - 确保ASC未被销毁
	 * 2. 解除委托绑定 - 防止残留回调
	 * 3. 生命周期管理 - 标记对象销毁
	 * 
	 * 调用时机：当UI元素销毁或不再需要监听时
	 */
  	UFUNCTION(BlueprintCallable)
  	void EndTask();
  protected:
  	
  
  	UPROPERTY()
  	TObjectPtr<UAbilitySystemComponent> ASC;
  	//记录监听的冷却标签
  	FGameplayTag CooldownTag;
	/**
	 * 监听冷却标签回调函数
	 * 
	 * 触发条件：
	 * - 冷却开始（NewCount > 0）
	 * - 冷却结束（NewCount == 0）
	 * 
	 * @param InCooldownTag - 发生变化的冷却标签
	 * @param NewCount - 标签当前计数（表示剩余冷却层数）
	 */
  	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	/**
	 * 监听ASC激活GE的回调
	 * 
	 * 监听GE应用事件来：
	 * - 检测新应用的冷却效果
	 * - 建立冷却时间更新委托
	 * 
	 * @param TargetASC - 应用效果的目标ASC
	 * @param SpecApplied - 应用的效果规格（包含冷却时长等信息）
	 * @param ActiveEffectHandle - 激活效果的句柄
	 */
  	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
  };
