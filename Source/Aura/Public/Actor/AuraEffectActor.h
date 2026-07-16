// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

// 用于表示效果应用的策略
UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	// 在重叠时应用效果
	ApplyOnOverlap,
	// 在结束重叠时应用效果
	ApplyOnEndOverlap,
	// 不应用效果
	DoNotApply
};

// 用于表示效果移除的策略
UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	// 在结束重叠时移除效果
	RemoveOnEndOverlap,
	// 不移除效果
	DoNotRemove
};

/**
 * 在场景中可放置的影响角色属性的物件基类
 */
UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();
	
	virtual void Tick(float DeltaSeconds) override;
protected:
	// 游戏开始或生成对象时回调
	virtual void BeginPlay() override;
	
	// 计算后的Actor所在的位置
	UPROPERTY(BlueprintReadWrite)
	FVector CalculatedLocation;

	// 计算后的Actor的旋转
	UPROPERTY(BlueprintReadWrite)
	FRotator CalculatedRotation;

	// Actor是否帧更新旋转
	UPROPERTY(BlueprintReadWrite, Category="Pickup Movement")
	bool bRotates = false;

	// Actor每秒旋转的角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float RotationRate = 45.f;

	// Actor是否更新位置
	UPROPERTY(BlueprintReadWrite, Category="Pickup Movement")
	bool bSinusoidalMovement = false;

	// 正弦值-1到1，此值为调整更新移动范围
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SineAmplitude = 1.f;

	// 此值参与正弦运算，默认值为1秒一个循环（2PI走完一个正弦的循环，乘以时间，就是一秒一个循环，可用于调整位置移动速度）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SinePeriod = 1.f; //2 * PI

	// 调用此函数，Actor开始自动更新上下位置
	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();

	// 调用此函数，Actor开始自动旋转
	UFUNCTION(BlueprintCallable)
	void StartRotation();

	// 给与目标添加GameplayEffect效果
	UFUNCTION(BlueprintCallable) 
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);//给与目标添加GameplayEffect

	// 在重叠开始时处理效果的添加删除逻辑
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	// 在重叠结束时处理效果的添加删除逻辑
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	//Instant和Duration的GE在应用后，此物体是否需要被销毁
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;

	//敌人是否能够拾取此物体
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	bool bApplyEffectsToEnemies = false;

	// 即时GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	// 持续GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	// 无限GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	// 使用 TWeakObjectPtr 避免悬空指针
	// UPROPERTY(Transient)
	// TMap<FActiveGameplayEffectHandle, TWeakObjectPtr<UAbilitySystemComponent>> ActiveEffectHandles;
	// 用于存储当前已经激活的GameplayEffect的句柄的map
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	float ActorLevel = 1.f;

private:

	//当前掉落物的存在时间，可以通过此时间实现动态效果
	float RunningTime = 0.f;

	// Actor生成的默认初始位置，在Actor动态浮动时，需要默认位置作为基础位置
	FVector InitialLocation;

	// 每一帧更新Actor的位置和转向
	void ItemMovement(float DeltaSeconds);
};
