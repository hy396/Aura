// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Abilities/ProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"



void UProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// const bool bIsServer = HasAuthority(&ActivationInfo);
	// if (!bIsServer) return;
	//
	// if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	// {
	// 	FTransform SpawnTransform;
	// 	SpawnTransform.SetLocation(CombatInterface->GetCombatSocketLocation());
	// 	SpawnTransform.SetRotation(GetAvatarActorFromActorInfo()->GetActorQuat());
	// 	//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
	// 	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
	// 		ProjectileClass,
	// 		SpawnTransform,
	// 		GetOwningActorFromActorInfo(),
	// 		Cast<APawn>(GetOwningActorFromActorInfo()),
	// 		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	//
	// 	//确保变换设置被正确应用
	// 	Projectile->FinishSpawning(SpawnTransform);
	// }
	
}

void UProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	const bool bOverridePitch, const float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //判断此函数是否在服务器运行
	if (!bIsServer) return;
	// 获取战斗接口的战斗插槽位置
	// 修改前：直接调用接口指针方法（存在蓝图实现不触发风险）
	//const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	// 使用Execute_前缀方法确保安全调用蓝图实现，注意：
	// 1. 优先调用蓝图覆盖版本，不存在时回退C++默认实现
	// 2. 自动进行接口有效性检查
	// 3. 适合可能被多线程访问的接口方法调用
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);

	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation(); // 将方向转为旋转
	//Rotation.Pitch = 0.f; //设置Pitch为0，转向的朝向将平行于地面
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	//SpawnTransform.SetRotation(GetAvatarActorFromActorInfo()->GetActorQuat());
	SpawnTransform.SetRotation(Rotation.Quaternion());
		
	//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	/*老方案
	// //创建一个GE的实例，并设置给投射物
	// const UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	// FGameplayEffectContextHandle EffectContextHandle = SourceAsc->MakeEffectContext();
	// EffectContextHandle.SetAbility(this); //设置技能
	// EffectContextHandle.AddSourceObject(Projectile); //设置GE的源
	// //添加Actor列表
	// TArray<TWeakObjectPtr<AActor>> Actors;
	// Actors.Add(Projectile);
	// EffectContextHandle.AddActors(Actors);
	// //添加命中结果
	// FHitResult HitResult;
	// HitResult.Location = ProjectileTargetLocation;
	// EffectContextHandle.AddHitResult(HitResult);
	// //添加技能触发位置
	// EffectContextHandle.AddOrigin(ProjectileTargetLocation);
	//
	// 	
	// const FGameplayEffectSpecHandle SpecHandle = SourceAsc->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	//
	// const FMyGameplayTags GameplayTags = FMyGameplayTags::Get(); //获取标签单列
	//
	// const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel()); //根据等级获取技能伤害
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);
	//
	// Projectile->DamageEffectHandle = SpecHandle;
	*/
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	//确保变换设置被正确应用
	Projectile->FinishSpawning(SpawnTransform);

}


