// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "PassiveNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UPassiveNiagaraComponent();

	//激活此被动技能特效的技能标签
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;

	/**
	 * 监听技能变动后的委托回调，用于设置此实例是否需要激活
	 * @param AbilityTag 对应的技能的标签
	 * @param bActivate 激活还是关闭
	 */
	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);

	/**
	 * 根据角色当前装备的被动技能状态，决定是否激活该 Niagara 特效组件。
	 * 此函数通常在组件初始化（如 BeginPlay）时调用，用于检查拥有者的能力系统中
	 * 是否已装备对应的被动技能标签（PassiveSpellTag），并据此触发或关闭特效。
	 *
	 * @param AuraASC 指向角色的 UAuraAbilitySystemComponent，用于查询当前装备的被动技能。
	 */
	void ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC);                    
};
