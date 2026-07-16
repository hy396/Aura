// Huanyu,miaomiaomiao,nekoneko


#include "Actor/EnemySpawnPoint.h"

#include "Character/AuraEnemy.h"

void AEnemySpawnPoint::SpawnEnemy()
{
	// 延迟生成Actor, 并设置其尝试调整位置但固定生成
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(
		EnemyClass,
		GetActorTransform(),
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	// 设置敌人的等级
	Enemy->SetLevel_Implementation(EnemyLevel);

	// 设置敌人角色的类型或职业
	Enemy->SetCharacterClass(CharacterClass);

	// 完成敌人的生成过程，并应用当前Actor的变换信息
	Enemy->FinishSpawning(GetActorTransform());
	
	// 生成并附加敌人的默认控制器（AI控制器或玩家控制器）
	Enemy->SpawnDefaultController();
}
