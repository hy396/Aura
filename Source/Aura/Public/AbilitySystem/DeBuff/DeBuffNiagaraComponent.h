// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DeBuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDeBuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:

	UDeBuffNiagaraComponent();
	// 用来标识粒子系统的标签
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DeBuffTag;

protected:
	virtual void BeginPlay() override;
	// 当前的负面标签变动回调
	void DeBuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	// 在角色死亡时的回调
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);

                        
};
