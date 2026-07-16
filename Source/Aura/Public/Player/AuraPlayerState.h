// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32 /*StatValue*/, bool /*bLevelUp*/)

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerState();

	// 指定某些属性需要在网络复制中进行生命周期管理
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCENOINLINE UAttributeSet* GetAttributeSet() const{ return AttributeSet;};

	// 设置升级相关数据
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	// 经验值变动委托
	FOnPlayerStatChanged OnXPChangedDelegate;

	// 等级变动委托
	FOnLevelChanged OnLevelChangedDelegate;

	// 属性点数变动委托
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;

	// 技能点数变动委托
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;

	// 获取等级
	FORCENOINLINE int32 GetPlayerLevel() const { return Level; };

	// 获取经验值
	FORCENOINLINE int32 GetXP() const { return XP; };

	// 获取属性点
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }

	// 获取技能点
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	
	// 获取经验值
	void AddToXP(int32 InXP);
	
	// 获取等级
	void AddToLevel(int32 InLevel);
	
	// 增加属性点
	void AddToAttributePoints(int32 InPoints);
	
	// 增加技能点
	void AddToSpellPoints(int32 InPoints);

	// 设置经验值
	void SetXP(int32 InXP);
	
	/**
	 * 设置当前等级
	 * @param InLevel 新的等级
	 * @param bLevelUp 当前是否提升了等级
	 */
	void SetLevel(int32 InLevel, const bool bLevelUp); 
	
	// 设置属性点
	void SetAttributePoints(int32 InPoints);
	
	// 设置技能点
	void SetSpellPoints(int32 InPoints);
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	// 属性同步 ReplicatedUsing
	// 等级
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	
	// 经验值
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 0;
	
	// 属性点
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	// 技能点
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 0;
	
	//服务器出现更改自动同步到本地函数 等级
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	//服务器出现更改自动同步到本地函数 经验值
	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
	
};
