// Huanyu,miaomiaomiao,nekoneko


#include "Checkpoint/Checkpoint.h"

#include "Aura/Aura.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 关闭帧更新
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(RootComponent);
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 设置查询并产生物理
	CheckpointMesh->SetCollisionResponseToChannels(ECR_Block); // 设置阻挡所有物体与其重叠
	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride); //设置自定义深度值
	CheckpointMesh->MarkRenderStateDirty(); //修改了材质，顶点，渲染可见性后，可以将其延后到帧结束，提交数据，下一帧更新，提升性能。

	// 设置球体碰撞体
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //设置其只用作查询使用
	Sphere->SetCollisionResponseToChannels(ECR_Ignore); // 设置其忽略所有物体
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 设置其与Pawn类型物体产生重叠事件

	//控制自动移动组件
	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		// 这个读档不知道为啥调用下面这个函数会失败, 导致mesh的材质丢失我也是服了
		//HandleGlowEffects();
		if(IsValid(ReachedMaterialInstance))
		{
			UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(ReachedMaterialInstance, this);
			CheckpointMesh->SetMaterial(0, DynamicMatInst);
		}
	}
}

void ACheckpoint::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
	//CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride);
}

void ACheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	// 在初始化中设置自定义深度值(构造处原本设置了的说, 不知道为什么不能成功，我也是服了我草了)
	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride); //设置自定义深度值

	//绑定重叠事件
	if (bBindOverlapCallback)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
	}
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		//如果检查点还未被激活，则进行让检查点高亮处理
		if (!bReached) HandleGlowEffects();
		
		// 设置当前检查点已被玩家激活
		bReached = true;

		if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			const UWorld* World = GetWorld();
			FString MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix);
			
			// 保存场景状态
			GameMode->SaveWorldState(GetWorld(), MapName);
		}
		
		// 修改存档当的检测点
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
	}
}

void ACheckpoint::HandleGlowEffects()
{
	//取消碰撞检查 不取消，可以每次触碰保存一次
	// Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//创建一个新材质实例，修改效果
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance); //触发检查点修改材质后的回调
}
