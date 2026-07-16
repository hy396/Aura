// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/DeBuff/DeBuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UAnimMontage;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface//, public IAbilitySystemReplicationProxyInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual void Tick(float DeltaTime) override;
	//设置生命周期内需要复制的属性
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/**
	 * 覆写 应用伤害给自身
	 * @see https://www.unrealengine.com/blog/damage-in-ue4
	 * @param DamageAmount		要施加的伤害数值
	 * @param DamageEvent		描述伤害细节的结构体，支持不同类型的伤害，如普通伤害、点伤害（FPointDamageEvent）、范围伤害（FRadialDamageEvent）等。
	 * @param EventInstigator	负责造成伤害的 Controller，通常是玩家或 AI 的控制器。
	 * @param DamageCauser		直接造成伤害的 Actor，例如爆炸物、子弹或掉落的石头。
	 * @return					返回实际应用的伤害值。这允许目标修改或减少伤害，然后将最终的值返回。
	 */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// 获取角色的AbilitySystemComponent
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// 获取角色的属性组件
	UAttributeSet* GetAttributeSet() const{ return AttributeSet;};

	/*	Combat Interface */
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die_Implementation(const FVector& DeathImpulse) override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void IncrementMinionCount_Implementation(const int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	virtual bool IsBeingShocked_Implementation() const override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual FOnDamageSignature& GetOnDamageSignature() override;
	/* end Combat Interface*/

	// ASC注册成功委托
	FOnASCRegistered OnAscRegistered;
	// 角色死亡后的委托
	FOnDeathSignature OnDeathDelegate;
	// 传入伤害后得到结果后的委托
	FOnDamageSignature OnDamageDelegate;
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<FTaggedMontage> AttackMontages;
	
	// 当前角色是否处于眩晕状态
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	// 当前角色是否处于燃烧状态
	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = false;
	
	// 当前角色是否处于持续攻击状态
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsBeingShocked = false;

	//设置当前敌人的职业类型
	void SetCharacterClass(const ECharacterClass InClass) { CharacterClass = InClass; }
	

	UFUNCTION()
	virtual void OnRep_Stunned();
	
	UFUNCTION()
	virtual void OnRep_Burned();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", 
	meta=(DisplayName="[溶解]死亡延迟", 
		 Tooltip="控制角色死亡后多长时间触发溶解效果 \n(建议值5-10秒)"))
	float DeathToDissolveDelay = 7.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(DisplayName="是否开启死亡物理模拟"))
	bool bEnablePhysicsSimulation = true;
	// 当前角色死亡状态
	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;
	
	//眩晕标签变动后的回调
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// 当前角色的最大移动速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat",
		meta=(DisplayName="移动速度",
		Tooltip="设置该角色的最大移动速度"))
	float BaseWalkSpeed = 600.f;


	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// 初始化角色技能组件函数，在子类的begin play中调用
	virtual void InitAbilityActorInfo();

	// 主要属性GE
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	// 次级属性GE
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	// 额外的属性GE
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;


	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, const float Level) const;

	// 初始化角色的属性
	virtual  void InitializeDefaultAttributes() const;

	// 初始化角色的技能
	void AddCharacterAbilities() const;

	// 溶解材质
	void Dissolve(); //溶解效果

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(const TArray<UMaterialInstanceDynamic*>& DynamicMaterialInstance);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	//受伤特效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;

	// 死亡音效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;

	//仆从数量
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	int32 MinionsCount = 0;
	
	// 火焰负面效果表现组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDeBuffNiagaraComponent> BurnDeBuffComponent;

	// 眩晕负面效果表现组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDeBuffNiagaraComponent> StunDeBuffComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults",
		meta=(DisplayName="职业",
		Tooltip="设置该角色的职业"))
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

private:

	// 角色初始技能设置
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	// 角色初始被动技能设置
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	//光环被动技能特效组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;

	//回血被动技能特效组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;

	//回蓝被动技能特效组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	//被动技能挂载的组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	
};
