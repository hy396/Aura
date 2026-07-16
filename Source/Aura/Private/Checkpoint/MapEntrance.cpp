// Huanyu,miaomiaomiao,nekoneko


#include "Checkpoint/MapEntrance.h"

#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"


AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AMapEntrance::LoadActor_Implementation()
{
	// 加载地图时,不做任何修改
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		// 保存场景状态
		if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			// 将目标关卡的路径传入, 并切换地图相关信息存入存档
			GameMode->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}

		// 将当前角色信息保存倒存档
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		// 切换地图
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
