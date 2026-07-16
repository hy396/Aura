// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "EnemySpawnVolume.generated.h"

UCLASS()
class AURA_API AEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnVolume();

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* Save Interface end */

	//当前怪物生成体积是否已经生成过敌人
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

	// 当前怪物生成体积是否可以反复利用
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanReuseSpawnVolume = false;

	// 当前怪物生成体积是否使用角色的等级生成
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUsePlayerLevelForSpawn = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//配置当前的生成体积所需生成的敌人
	UPROPERTY(EditAnywhere)
	TArray<AEnemySpawnPoint*> SpawnPoints;
private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;
};
