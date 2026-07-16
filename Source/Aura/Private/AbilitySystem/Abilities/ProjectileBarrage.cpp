// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Abilities/ProjectileBarrage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"


FString UProjectileBarrage::GetDescription(int32 Level)
{
	return GetDescriptionAtLevel(Level,L"超位魔法轰炸");
}

FString UProjectileBarrage::GetNextLevelDescription(int32 Level)
{
	return GetDescriptionAtLevel(Level,L"下一等级");
}

FString UProjectileBarrage::GetDescriptionAtLevel(int32 Level, const FString& Title)
{
	const int32 Damage = ProjectileData[0].DamageTypes[FMyGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// 标题
		"<Title>%s</>\n"

		// 细节
		"<Small>Level:</> <Level>%i</>\n"
		"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
		"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

		// 技能描述
		"<Default>多方向进行发射 %i 颗魔法飞弹，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点魔法伤害。</>"),

		// 动态修改值
		*Title,
		Level,
		Cooldown,
		ManaCost,
		NumMinions,
		Damage);
}

FDamageEffectParams UProjectileBarrage::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector KnockbackDirectionOverride,
	bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	FProjectileType QwQ = ProjectileData[Index];
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = QwQ.DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	for (TTuple<FGameplayTag, FScalableFloat> Pair: QwQ.DamageTypes)
	{
		Params.BaseDamage += Pair.Value.GetValueAtLevel(GetAbilityLevel()); //根据等级获取技能伤害
		Params.DamageType = Pair.Key;
		
	}
	Params.AbilityLevel = GetAbilityLevel();

	Params.DeBuffChance = QwQ.DeBuffChance;
	Params.DeBuffDamage = QwQ.DeBuffDamage;
	Params.DeBuffDuration = QwQ.DeBuffDuration;
	Params.DeBuffFrequency = QwQ.DeBuffFrequency;
	Params.DeathImpulseMagnitude = QwQ.DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = QwQ.KnockbackForceMagnitude;
	Params.KnockbackChance = QwQ.KnockbackChance;


	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		const FVector ToTarget = Rotation.Vector();
		if (!bOverrideKnockbackDirection)
		{
			Params.KnockbackForce = ToTarget * QwQ.KnockbackForceMagnitude;
		}
		if (!bOverrideDeathImpulse)
		{
			Params.DeathImpulse = ToTarget * QwQ.DeathImpulseMagnitude;
		}
	}
	
	
	if (bOverrideKnockbackDirection)
	{
		KnockbackDirectionOverride.Normalize();
		Params.KnockbackForce = KnockbackDirectionOverride * QwQ.KnockbackForceMagnitude;
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;
			Params.KnockbackForce = KnockbackRotation.Vector() * QwQ.KnockbackForceMagnitude;
		}
	}

	if (bOverrideDeathImpulse)
	{
		DeathImpulseDirectionOverride.Normalize();
		Params.DeathImpulse = DeathImpulseDirectionOverride * QwQ.DeathImpulseMagnitude;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;
			Params.DeathImpulse = DeathImpulseRotation.Vector() * QwQ.DeathImpulseMagnitude;
		}
	}
	
	if (QwQ.bIsRadialDamage)
	{
		Params.bIsRadialDamage = QwQ.bIsRadialDamage;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
		Params.RadialDamageInnerRadius = QwQ.RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = QwQ.RadialDamageOuterRadius;
	}
	
	return Params;
}

void UProjectileBarrage::SpawnProjectiles(const FVector& InLocation,const FVector& ProjectileTargetLocation,
	const bool bOverridePitch, const float PitchOverride, AActor* HomingTarget)
{
	// 判断此函数是否在服务器运行
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	Index = GetRandomProjectile();
	// 将方向转为旋转
	FRotator Rotation = (ProjectileTargetLocation - InLocation).Rotation(); // 将方向转为旋转
	// 覆写发射角度
	if (bOverridePitch) Rotation.Pitch = PitchOverride;
	//遍历所有朝向，并生成火球术
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(InLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
				
		//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileData[Index].ProjectileClass,
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

}

TArray<FVector> UProjectileBarrage::GetSpawnLocations()
{
	// // 获取召唤者的朝向
	// const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	// const FVector Up = GetAvatarActorFromActorInfo()->GetActorUpVector();
	// // 获取召唤者的位置
	// FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	// // 提升高度
	// Location += FVector(0.f,0.f,LocationHeight);
	// const FVector BackLocation = Up.RotateAngleAxis(-60.f,Forward);
	// TArray<FVector> SpawnLocations;
	// const float DeltaSpread = SpawnSpread / NumMinions;
	// for (int32 i = 0; i < NumMinions; ++i)
	// {
	// 	// 计算当前单位的角度方向（从左侧开始按间隔旋转）
	// 	const FVector TempVector = BackLocation.RotateAngleAxis(DeltaSpread * i, Up);
	// 	// 转换一下角度
	// 	const FVector Direction = TempVector.RotateAngleAxis(90.f,Forward);
	// 	
	// 	// 在最小/最大距离之间随机生成位置
	// 	FVector ChosenSpawnLocation = Location + TempVector * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
	// 	UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, ChosenSpawnLocation, 4.f, FLinearColor::Red, 3.f);
 //
	// 	// 执行射线检测找地面位置
	// 	// FHitResult Hit;
	// 	// GetWorld()->LineTraceSingleByChannel(
	// 	// 	Hit, 
	// 	// 	ChosenSpawnLocation + FVector(0.f, 0.f, 400.f),  // 检测起点（上方400个单位）
	// 	// 	ChosenSpawnLocation - FVector(0.f, 0.f, 400.f),  // 检测终点（下方400个单位）
	// 	// 	ECC_Visibility);
 //  //   
	// 	// // 如果检测到地面，使用碰撞点作为生成位置
	// 	// if (Hit.bBlockingHit)
	// 	// {
	// 	// 	ChosenSpawnLocation = Hit.ImpactPoint;
	// 	// }
	// 	// if (ChosenSpawnLocation.Z < 0)
	// 	// {
	// 	// 	ChosenSpawnLocation.Z = 0;
	// 	// 	ChosenSpawnLocation += FVector(0.f,0.f,LocationHeight/2);
	// 	// }
	// 	SpawnLocations.Add(ChosenSpawnLocation);
	// }
	// 获取角色后方方向（角色正后方）
	const FVector Backward = -GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector UpVector = GetAvatarActorFromActorInfo()->GetActorUpVector(); // 获取角色上方
	// 设置仰角范围（例如30度仰角）
	const float ElevationAngle = 30.0f; // 仰角角度
	// 在后方扇形基础上增加垂直方向分布
	const FVector BaseDirection = (Backward.RotateAngleAxis(-ElevationAngle, FVector::RightVector)).GetSafeNormal();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FVector> SpawnLocations;
	const float DeltaSpread = SpawnSpread / NumMinions;
	// 调整后的主方向（带仰角的基础方向）
	const FVector BaseDir = BaseDirection.RotateAngleAxis(-SpawnSpread/2, UpVector); 
	for(int32 i = 0; i < NumMinions; ++i)
	{
		// 水平偏移
		const float HorizontalAngle = -(SpawnSpread/5)/2 + i*(DeltaSpread/5);
		// 垂直方向随机偏移（-10到+10度）
		const float VerticalOffset = FMath::FRandRange(-100.0f, 100.0f);
        /*
		// 生成方向（带水平偏移和垂直偏移
		// FVector Dir = BaseDir.RotateAngleAxis(HorizontalAngle, UpVector) 
		// 			 * FRotator(VerticalOffset, 0, 0).Vector();
        */
		const FVector Direction = BaseDir.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		const float RandDistance = FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		FVector SpawnPoint = Location + Direction * RandDistance;
		/*
		// 	// 计算当前单位的角度方向（从左侧开始按间隔旋转）
		// 	const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		// 	// 在最小/最大距离之间随机生成位置
		// 	FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
        
		// 投射到地面（同原逻辑）
		// FHitResult Hit;
		// GetWorld()->LineTraceSingleByChannel(
		// 	Hit, 
		// 	SpawnPoint + FVector(0.f, 0.f, 400.f),  // 检测起点（上方400个单位）
		// 	SpawnPoint - FVector(0.f, 0.f, 400.f),  // 检测终点（下方400个单位）
		// ECC_Visibility);
		//
		// // 如果检测到地面，使用碰撞点作为生成位置
		// if (Hit.bBlockingHit)
		// {
		// 	SpawnPoint = Hit.ImpactPoint;
		// }
		//...（原投射检测代码）
		*/
		SpawnPoint += FVector(HorizontalAngle, VerticalOffset, RandDistance);
		//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, SpawnPoint, 4.f, FLinearColor::Red, 3.f);
		SpawnLocations.Add(SpawnPoint);
	}
	Test::Shuffle(SpawnLocations);
	return SpawnLocations;
}

void UProjectileBarrage::SpawnProjectile(const FVector& InLocation, const FVector& ProjectileTargetLocation,
	  const bool bOverridePitch, const float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //判断此函数是否在服务器运行
	if (!bIsServer) return;
	// 获取随机的ProjectileData
	Index = GetRandomProjectile();
	FRotator Rotation = (ProjectileTargetLocation - InLocation).Rotation(); // 将方向转为旋转
	//Rotation.Pitch = 0.f; //设置Pitch为0，转向的朝向将平行于地面
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(InLocation);
	//SpawnTransform.SetRotation(GetAvatarActorFromActorInfo()->GetActorQuat());
	SpawnTransform.SetRotation(Rotation.Quaternion());
		
	//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileData[Index].ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	//确保变换设置被正确应用
	Projectile->FinishSpawning(SpawnTransform);
}