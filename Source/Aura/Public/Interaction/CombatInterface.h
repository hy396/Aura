// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Components/SphereComponent.h"
#include "CombatInterface.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnASCRegistered, UAbilitySystemComponent*)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageSignature, float  /*范围伤害造成的最终数值*/); //返回范围伤害能够对自身造成的伤害，在TakeDamage里广播

class UAnimMontage;

//蒙太奇动画和标签以及骨骼位置的映射，用于攻击技能获取和设置攻击范围
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	// 使用的蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	// 蒙太奇钟对应的标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	// 骨骼的标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;
	
	// 攻击时的触发音效
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetPlayerLevel();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="获取技能释放位置，通过在蓝图中设置获取WeaponTipSocketName的位置"))
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,
		meta=(Tooltip="获取位置 -> 旋转朝向"))
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="获取受击蒙太奇动画"))
	UAnimMontage* GetHitReactMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="获取武器"))
	USkeletalMeshComponent* GetWeapon();
	
	/**
	 * 获取角色注册成功的广播委托
	 * @return 返回委托
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Die(const FVector& DeathImpulse);

	/**
	 * 获取角色死亡触发的委托
	 * @return 委托
	 */
	virtual FOnDeathSignature& GetOnDeathDelegate() = 0;

	/**
	 * 获取角色受到伤害触发的委托，由于委托是创建在角色基类里的，这里可以通过添加struct来实现前向声明，不需要在头部声明一遍。
	 * @return 委托
	 */
	virtual FOnDamageSignature& GetOnDamageSignature() = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="获取当前角色是否死亡"))
	bool IsDead() const; 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="获取当前角色"))
	AActor* GetAvatar(); 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="获取攻击蒙太奇动画数组,该数组存储的是'FTaggedMontage'"))
	TArray<FTaggedMontage> GetAttackMontages();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="返回该角色的受伤特效"))
	UNiagaraSystem* GetBloodEffect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="通过标签获取对应的结构体'FTaggedMontage'"))
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="获取角色拥有的仆从数量"))
	int32 GetMinionCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
		meta=(Tooltip="设置角色仆从的增长数量，负数为负增长"))
	void IncrementMinionCount(const int32 Amount);

	// 获取当前角色的职业
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass();
	
	// 获取ASC注册成功后的委托
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() = 0; 

	/**
	 * 设置角色是否在循环施法中
	 * @param bInLoop 开启或关闭角色循环施法状态
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetInShockLoop(bool bInLoop);

	/**
	 * 获取角色是否处于闪电链攻击状态
	 * @return 布尔值，如果处于返回true
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsBeingShocked() const;

	/**
	 * 设置角色是否处于闪电链攻击状态
	 * @param bInShock 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIsBeingShocked(bool bInShock);

	/**
	 * 返回角色碰撞球体
	 * @return 碰撞球体
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USphereComponent* GetDunSphereComponent();

	/**
	* 设置球体的碰撞
	* @param bInCollision 是否开启碰撞
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetDunSphereComponent(bool bInCollision);
};
