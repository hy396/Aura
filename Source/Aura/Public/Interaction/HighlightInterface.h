// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//高亮 描边
	UFUNCTION(BlueprintNativeEvent)
	void HighlightActor();
	
	//取消高亮 描边
	UFUNCTION(BlueprintNativeEvent)
	void UnHighlightActor();

	/**
	 * 设置角色移动的目标位置
	 * @param OutDestination 引用，调用将目标位置设置到此值
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SetMoveToLocation(FVector& OutDestination);
};
