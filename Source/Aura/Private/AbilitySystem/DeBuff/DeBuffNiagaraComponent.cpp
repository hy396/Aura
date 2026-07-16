// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/DeBuff/DeBuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDeBuffNiagaraComponent::UDeBuffNiagaraComponent()
{
	bAutoActivate = false; //关闭自动激活
}

void UDeBuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	// 获取到战斗接口
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	// 通过函数库获取角色身上的ASC
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		// 监听负面标签变动回调
		ASC->RegisterGameplayTagEvent(DeBuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDeBuffNiagaraComponent::DeBuffTagChanged);
	}
	else if (CombatInterface)//如果绑定时，ASC未初始化成功，则监听ASC创建完成委托，完成对负面标签的监听
	{
		//AddWeakLambda 这种绑定方式的主要好处是，当绑定的对象被销毁时，委托不会保持对象的引用，从而避免悬空指针问题和内存泄漏。
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			// 监听负面标签变动回调
			InASC->RegisterGameplayTagEvent(DeBuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDeBuffNiagaraComponent::DeBuffTagChanged);
		});
	}
	// 绑定死亡后销毁
	if(CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDeBuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDeBuffNiagaraComponent::DeBuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bOwnerValid = IsValid(GetOwner());
	const bool bOwnerAlive = GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner());
	
	if (NewCount > 0 && bOwnerValid && bOwnerAlive)
	{
		// 激活Niagara组件
		Activate();
	}
	else
	{
		// 停用Niagara组件
		Deactivate();
	}
}

void UDeBuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
