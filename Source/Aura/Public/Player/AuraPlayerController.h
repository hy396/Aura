// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/InputConfig.h"
#include "Input/AuraInputComponent.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Interaction/HighlightInterface.h"
#include "UI/Widget/DamageTextComponent.h"

#include "AuraPlayerController.generated.h"

class USplineComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;

//鼠标拾取目标的状态枚举
enum class ETargetingStatus : uint8
{
	//敌人
	TargetingEnemy,
	//鼠标拾取的目标非敌人
	TargetingNonEnemy,
	//无
	NotTargeting
};


/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	// 在每个客户端显示伤害数值
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit, bool bIsExp);

	//显示魔法光圈 并设置光圈贴花材质
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial);

	//隐藏魔法光圈
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle() const; 

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void Move(const FInputActionValue& InputActionValue);
	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;

	// 按下
	void AbilityInputTagPressed(FGameplayTag InputTag);
	// 抬起
	void AbilityInputTagReleased(FGameplayTag InputTag);
	// 将在每一帧触发
	void AbilityInputTagHeld(FGameplayTag InputTag);



	/**
	 * 进行光标追踪的函数。
	 * 
	 * 该函数用于在游戏中进行光标追踪，并根据追踪结果高亮显示相应的Actor。
	 * 
	 * 功能描述：
	 * 1. 检查是否存在Gameplay Ability System组件以及是否具有阻止光标追踪的Gameplay Tag。
	 * 2. 如果存在阻止光标追踪的Gameplay Tag，则取消高亮显示上一个和当前的Actor，并重置相关变量。
	 * 3. 根据是否存在MagicCircle决定使用的碰撞通道。
	 * 4. 获取光标下的碰撞结果，如果没有碰撞则返回。
	 * 5. 更新上一个和当前的Actor，并根据是否实现了高亮接口来决定是否高亮显示。
	 * 6. 如果上一个和当前的Actor不同，则取消高亮显示上一个Actor并高亮显示当前Actor。
	 * 
	 * 该函数假设存在UnHighlightActor和HighlightActor函数用于处理Actor的高亮显示。
	 */
	void CursorTrace(); //鼠标位置追踪拾取
	TObjectPtr<AActor> LastActor; //上一帧拾取到的接口指针
	TObjectPtr<AActor> ThisActor; //这一帧拾取到的接口指针
	FHitResult CursorHit; // 声明一个FHitResult类型的变量，用于存储碰撞结果
	static void HighlightActor(AActor* InActor);
	static void UnHighlightActor(AActor* InActor);
	
	UFUNCTION(BlueprintPure)
	FHitResult GetCursorHit();

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputConfig> InputConfig;
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector; //存储鼠标点击的位置
	float FollowTime = 0.f; // 用于查看按住了多久
	bool bAutoRunning = false; //当前是否自动移动
	//bool bTargeting = false; //当前鼠标是否选中敌人
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting; //鼠标拾取

	// 定义鼠标悬停多长时间内算点击事件
	UPROPERTY(EditDefaultsOnly)
	float ShortPressThreshold = 0.3f; 

	// 当角色和目标距离在此半径内时，将关闭自动寻路
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	// 自动寻路时生成的样条线
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline; 

	// 点击时生成的Niagara系统
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	// 创建奥数光圈使用的类
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	// 存储魔法光圈的属性，不需要暴露给蓝图
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;
	
	// 每一帧调用，更新魔法光圈的位置
	void UpdateMagicCircleLocation() const; 
};
