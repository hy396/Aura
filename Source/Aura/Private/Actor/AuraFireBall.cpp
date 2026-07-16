// Huanyu,miaomiaomiao,nekoneko


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MyGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"

// Called when the game starts or when spawned
void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	
	StartOutgoingTimeline(); //调用开始时间线修改
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 检查重叠有效性，若无效则提前退出
	if (!IsValidOverlap(OtherActor)) return;
	
	// 仅在服务器端执行伤害逻辑
	if (HasAuthority())
	{
		// 获取目标Actor的Ability System组件
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// 受击力设置
			const FVector KnockbackDirection = GetActorForwardVector().RotateAngleAxis(-45.f, GetActorRightVector());
			DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;

			// 根据火球前进方向计算死亡冲击力
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			
			// 设置目标Ability System组件并应用伤害效果
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AAuraFireBall::OnHit()
{
	// 若拥有者存在，触发火球爆炸的Gameplay效果
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation(); // 设置效果触发位置为火球当前位置
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated( // 非网络同步触发Gameplay cue
			GetOwner(), 
			FMyGameplayTags::Get().GameplayCue_FireBlast, // 使用预定义的"火球爆炸"效果标签
			CueParams
		);
	}

	// 停止并销毁持续播放的声音组件
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	// 标记火球已命中目标
	bHit = true;
}


