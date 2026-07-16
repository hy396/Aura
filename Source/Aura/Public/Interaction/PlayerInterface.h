// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// 根据经验获取等级
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;

	// 获取当前经验值
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	// 获取属性点奖励
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 Level) const;

	// 获取技能点奖励
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 Level) const;

	// 增加经验
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	// 增加等级
	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);

	// 增加属性点
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);

	// 获取可分配属性点数
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	// 增加技能点
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);

	// 获取可分配技能点数
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	// 升级
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
	
	// 显示魔法光圈，并设置魔法光圈的贴花材质
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	// 隐藏魔法光圈
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
	
	/**
	 * 保存游戏进度
	 * @param CheckpointTag 当前保存存档时的检查点的Tag
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SaveProgress(const FName& CheckpointTag);

	// 获取角色使用的次级属性GameplayEffect
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TSubclassOf<UGameplayEffect> GetSecondaryAttributes();

	// 获取角色使用的额外属性GameplayEffect
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TSubclassOf<UGameplayEffect> GetVitalAttributes();
};
