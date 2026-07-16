// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "Actor/AuraProjectile.h"
#include "GameplayEffect.h"
#include "Aura/Public/MyGameplayTags.h"
#include "ProjectileSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()


protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category="Projectile", 
	meta=(Tooltip="生成发射物品.瞄准传入的目标位置,通过传入的骨骼标签来寻找发射位置"))
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,const bool bOverridePitch = false,const float PitchOverride = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;

};
