// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	// 是否由客户端控制
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();

	if (bIsLocallyControlled)
	{
		// 如果是客户端控制器控制，实现将数据发射到服务器端
		SendMouseCursorData();
	}else
	{
		// 获取游戏技能规格句柄
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		// 获取激活预测键
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		// 将目标数据集委托添加到技能系统组件中
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		// 判断在服务器端，上面的委托是否已经广播过
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if(!bCalledDelegate)
		{
			// 设置服务器端等待PlayerData数据的上传
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// 创建一个预测窗口，该窗口允许客户端在不确定服务器响应的情况下，对游戏状态进行预测性更新。
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), true);

	// 获取鼠标拾取结果
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);

	// 创建需要上传服务器端的TargetData
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	
	// 创建TargetData句柄，上传到服务器端需要上传句柄
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);
	
	// 将TargetData上传到服务器端
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	//判断服务器端是否通过验证
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(
	const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// 通知客户端 服务器端已经接收并处理了从客户端复制的目标数据（将服务器的TargetData应用到客户端，并清除掉缓存）
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	// 判断服务器端是否通过验证
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
