// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Abilities/AuraSummonAbility.h"


TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	// 获取召唤者的朝向
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	// 获取召唤者的位置
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	// 将召唤的角度范围进行分段，在每段里面生成一个召唤物
	const float DeltaSpread = SpawnSpread / NumMinions;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector); //获取到最左侧的角度
	//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + LeftOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Red, 3.f);
	//const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector); //获取到最右侧的角度
	//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + RightOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Red, 3.f);

	TArray<FVector> SpawnLocations;
	// 按召唤物数量循环生成每个单位的位置
	for (int32 i = 0; i < NumMinions; ++i)
	{
		// 计算当前单位的角度方向（从左侧开始按间隔旋转）
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		// 在最小/最大距离之间随机生成位置
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
    
		// 执行射线检测找地面位置
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit, 
			ChosenSpawnLocation + FVector(0.f, 0.f, 400.f),  // 检测起点（上方400个单位）
			ChosenSpawnLocation - FVector(0.f, 0.f, 400.f),  // 检测终点（下方400个单位）
			ECC_Visibility);
    
		// 如果检测到地面，使用碰撞点作为生成位置
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(ChosenSpawnLocation);
	}
	
	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}