// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 火焰爆发使用的火球类
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()

public:
	// 执行蓝图时间轴事件，需要在蓝图中实现此事件
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();
	
	// 当前火球返回的目标角色，默认是技能的释放者，在创建火球是创建
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;
	// 火球爆炸造成的伤害
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
	
protected:
	virtual void BeginPlay() override;
	
	/**
	 * 发射物发生碰撞后的回调函数
	 * @param OverlappedComponent 发生重叠事件的自身的碰撞体对象
	 * @param OtherActor 目标的actor对象
	 * @param OtherComp 目标的碰撞体组件
	 * @param OtherBodyIndex 目标身体的索引
	 * @param bFromSweep 是否为瞬移检测到的碰撞
	 * @param SweepResult 如果位置发生过瞬移（直接设置到某处），两个位置中间的内容会记录到此对象内
	 */
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnHit() override;
};
