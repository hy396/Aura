// Huanyu,miaomiaomiao,nekoneko


#include "Actor/EnemySpawnVolume.h"

#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"

// Sets default values
AEnemySpawnVolume::AEnemySpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //仅用于检测
	Box->SetCollisionObjectType(ECC_WorldStatic); //设置物体类型
	Box->SetCollisionResponseToChannels(ECR_Ignore); //忽略所有检测通道
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //开启与pawn的重叠检测
}

void AEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
	{
		Destroy();
	}
}

// Called when the game starts or when spawned
void AEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnVolume::OnBoxOverlap);
	
}

void AEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;

	if (bUsePlayerLevelForSpawn && !OtherActor->Implements<UCombatInterface>()) return;
	
	bReached = true;
	// 仅在服务器生成敌人
	if(HasAuthority())
	{
		// 在设置的所有点位生成敌人
		for (AEnemySpawnPoint* SpawnPoint : SpawnPoints)
		{
			if (IsValid(SpawnPoint))
			{
				if (bUsePlayerLevelForSpawn)
				{
					SpawnPoint->EnemyLevel = ICombatInterface::Execute_GetPlayerLevel(OtherActor);
				}
				SpawnPoint->SpawnEnemy();
			}
		}
	}


	if (bCanReuseSpawnVolume)
	{
		bReached = false;
	}else
	{
		// 设置在不产生物理查询,直接销毁无法保存到存档
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


