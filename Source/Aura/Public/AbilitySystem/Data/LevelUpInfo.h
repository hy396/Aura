// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	// 升到此等级所需经验值
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;
	// 达到此等级奖励的属性点值
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;
	// 达到此等级降级的可分配技能点数
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	// 当前所有等级的升级数据
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;
	// 通过经验值值获取角色的等级
	int32 FindLevelForXP(int32 XP) const;
};
