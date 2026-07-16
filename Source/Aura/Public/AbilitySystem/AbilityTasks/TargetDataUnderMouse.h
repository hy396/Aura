// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

// 声明一个动态多播委托，该委托接受一个参数，参数类型为const FVector&
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);


/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
public:

	/**
	 * DisplayName 为在蓝图搜索时，可以直接搜索这个名称，节点上也会显示这个名称。
	 * HidePin 是隐藏一个参数的引脚，设置以后，在蓝图中无法设置它的属性。
	 * DefaultToSelf 将类或者蓝图实例作为默认参数 这两项一起使用，我们就不需要设置OwningAbility的值了，默认设置了蓝图实例。
	 * BlueprintInternalUseOnly 设置了此函数只能在蓝图中使用。
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	//BlueprintAssignable表示该属性可以在蓝图编辑器中进行赋值操作，通常用于事件或委托。
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

private:
	virtual void Activate() override;
	// 客户端向服务器端提交数据
	void SendMouseCursorData();
	// 当数据提交到服务器端后的委托回调
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);



};
