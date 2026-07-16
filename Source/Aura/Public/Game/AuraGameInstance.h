// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	// 角色进入关卡后默认生成的PlayerStart的Tag
	UPROPERTY()
	FName PlayerStartTag = FName();

	// 当前使用或后续保存内容到的存档名称
	UPROPERTY()
	FString LoadSlotName = FString();

	// 当前使用或后续保存的存档索引
	UPROPERTY()
	int32 LoadSlotIndex = 0;
};
