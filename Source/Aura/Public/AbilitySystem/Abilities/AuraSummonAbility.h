// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();
	
	UFUNCTION(BlueprintPure, Category="Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
	meta=(Tooltip="召唤的数量"))
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
	meta=(Tooltip="召唤生成的角色类"))
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
	meta=(Tooltip="召唤物距离召唤师最近的距离"))
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
	meta=(Tooltip="召唤物距离召唤师最远的距离"))
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Summoning", 
	meta=(Tooltip="召唤物在召唤师前面的角度范围"))
	float SpawnSpread = 90.f;
	
};
