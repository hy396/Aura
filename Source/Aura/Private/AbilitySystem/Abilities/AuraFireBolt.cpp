// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Public/MyGameplayTags.h"


FString UAuraFireBolt::GetDescription_Implementation(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"火球术");
}

FString UAuraFireBolt::GetNextLevelDescription_Implementation(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"下一等级");
}

FString UAuraFireBolt::GetDescriptionAtLevel(int32 Level, const FString& Title)
{
	// 伤害
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	// 法力消耗
	const float ManaCost = GetManaCost(Level);
	// 技能冷却
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// 标题
		"<Title>%s</>\n"

		// 细节
		"<Small>Level:</> <Level>%i</>\n"
		"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
		"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

		// 技能描述
		"<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),

		// 动态修改值
		*Title,
		Level,
		Cooldown,
		ManaCost,
		FMath::Min(Level, NumProjectiles),
		ScaledDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	 const bool bOverridePitch, const float PitchOverride, AActor* HomingTarget)
{
	// 判断此函数是否在服务器运行
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	// 获取释放位置
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);
	// 将方向转为旋转
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	// 覆写发射角度
	if (bOverridePitch) Rotation.Pitch = PitchOverride;
	// 获取朝向向量
	const FVector Forward = Rotation.Vector();
	// 限制产生火球的最大数量
	const int32 EffectiveNumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
	// 根据函数获取到所有生成的转向
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, EffectiveNumProjectiles);
	//遍历所有朝向，并生成火球术
	for(FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());
				
		//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		//根据目标类型设置HomingTargetComponent，此内容必须在飞弹被生成后设置
		if(HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			//设置攻击的位置为攻击对象的根位置
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			//如果没有获取到攻击目标，则创建一个可销毁的并应用
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation); //设置组件位置
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		//设置飞弹朝向目标时的加速度
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles; //设置为true，飞弹将加速飞向攻击目标

		//确保变换设置被正确应用
		Projectile->FinishSpawning(SpawnTransform);

		//Debug
		//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + Rot.Vector() * 100.f, 5, FLinearColor::Green, 120, 5);
	}
	
}
