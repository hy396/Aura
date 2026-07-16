// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,
		meta=(Keywords = "设置战斗目标",
			Tooltip="设置传入'Actor'为攻击目标"))
	void SetCombatTarget(AActor* InCombatTarget);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,
		meta=(Keywords = "获取战斗目标",
			Tooltip="获取已设置好的'Actor','\n'注:要先设置目标再获取目标哦~"))
	AActor* GetCombatTarget() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetLevel(const int32 InLevel);
};
