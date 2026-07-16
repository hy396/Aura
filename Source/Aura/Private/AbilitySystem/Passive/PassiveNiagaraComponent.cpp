// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	// 关闭自动激活
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		// // 在初始化时，读取存档已经结束，技能也设置完成，所以我们需要读取存档并更新显示
		// if (AuraASC->GetStatusTagFromAbilityTag(PassiveSpellTag) == FMyGameplayTags::Get().Abilities_Status_Equipped)
		// {
		// 	OnPassiveActivate(PassiveSpellTag, true);
		// }
		ActivateIfEquipped(AuraASC);
	}else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		//AddWeakLambda 这种绑定方式的主要好处是，当绑定的对象被销毁时，委托不会保持对象的引用，从而避免悬空指针问题和内存泄漏。
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this,[this](UAbilitySystemComponent* InASC)
		{
			// 确保注册的ASC是AuraAbilitySystemComponent类型
			if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(InASC))
			{
				// 绑定被动效果激活事件到OnPassiveActivate回调
				AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				// // 在初始化时，读取存档已经结束，技能也设置完成，所以我们需要读取存档并更新显示
				// if (AuraASC->GetStatusTagFromAbilityTag(PassiveSpellTag) == FMyGameplayTags::Get().Abilities_Status_Equipped)
				// {
				// 	OnPassiveActivate(PassiveSpellTag, true);
				// }
				ActivateIfEquipped(AuraASC);
			}
		});
                        
	}

}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	// 判断技能标签是否匹配
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		// 判断是否需要激活
		if (bActivate)
		{
			//不需要重复激活
			if(!IsActive()) Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

void UPassiveNiagaraComponent::ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC)
{
	//const bool bStartupAbilitiesGiven = AuraASC->bStartupAbilitiesGiven;
	if (AuraASC->bStartupAbilitiesGiven)
	{
		if (AuraASC->GetStatusTagFromAbilityTag(PassiveSpellTag) == FMyGameplayTags::Get().Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}
