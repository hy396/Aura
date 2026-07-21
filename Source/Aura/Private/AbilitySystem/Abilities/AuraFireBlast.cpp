// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

FString UAuraFireBlast::GetDescription_Implementation(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"火焰爆发");
}

FString UAuraFireBlast::GetNextLevelDescription_Implementation(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"下一等级");
}

FString UAuraFireBlast::GetDescriptionAtLevel(int32 Level, const FString& Title)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(
		TEXT(
		// 标题
		"<Title>%s</>\n"

		// 细节
		"<Small>等级：</> <Level>%i</>\n"
		"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
		"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

		// 技能描述
		"<Default>向四面八方发射 %i 颗火球，每颗火球会在返回时发生爆炸，并造成</> <Damage>%i</> <Default>点径向火焰伤害，并有一定几率触发燃烧。</>"),

		// 动态修改值
		*Title,
		Level,
		Cooldown,
		-ManaCost,
		NumFireBalls,
		ScaledDamage
	);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	// 获取到角色的朝向和位置
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	// 通过函数获取到每个需要生成的火球的旋转
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);

	// 生成火球
	TArray<AAuraFireBall*> FireBalls;

	for (const FRotator& Rotator : Rotators)
	{
		// 创建变换
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		
		// 创建火球 使用 SpawnActorDeferred 来生成对象时，UE 会延迟实际的对象生成过程，这样你有机会在完全初始化对象之前进行自定义配置。
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		//设置火球的伤害配置
		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		//创建火球销毁时的爆炸伤害配置
		FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();

		//设置火球的所有者为技能释放者
		FireBall->SetOwner(GetAvatarActorFromActorInfo());
		
		FireBalls.Add(FireBall);

		//在配置完成火球配置后，调用FinishSpawning将火球正式添加到场景中
		FireBall->FinishSpawning(SpawnTransform);
	}

	return FireBalls;
}
