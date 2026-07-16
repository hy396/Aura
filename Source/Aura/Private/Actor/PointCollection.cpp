// Huanyu,miaomiaomiao,nekoneko


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APointCollection::APointCollection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//设置Pt_0作为根节点
	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);

	//将其它的场景组件作为根组件的子节点添加，会在蓝图中去修改它的位置
	CreateSceneComponent(Pt_1, "Pt_1");
	CreateSceneComponent(Pt_2, "Pt_2");
	CreateSceneComponent(Pt_3, "Pt_3");
	CreateSceneComponent(Pt_4, "Pt_4");
	CreateSceneComponent(Pt_5, "Pt_5");
	CreateSceneComponent(Pt_6, "Pt_6");
	CreateSceneComponent(Pt_7, "Pt_7");
	CreateSceneComponent(Pt_8, "Pt_8");
	CreateSceneComponent(Pt_9, "Pt_9");
	CreateSceneComponent(Pt_10, "Pt_10");

}

// Called when the game starts or when spawned
void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}

void APointCollection::CreateSceneComponent(TObjectPtr<USceneComponent>& Pt, FName Name)
{
	Pt = CreateDefaultSubobject<USceneComponent>(Name);
	ImmutablePts.Add(Pt);
	Pt->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints,
	float YawOverride)
{
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("访问索引超过了数组的范围"));

	TArray<USceneComponent*> ArrayCopy;

	for (USceneComponent* Pt : ImmutablePts)
	{
		if (ArrayCopy.Num() >= NumPoints) return ArrayCopy;

		if (Pt != Pt_0)
		{
			// 获取到节点基于根组件世界坐标系下的偏移
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			// 对偏移值进行垂直偏移
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			// 设置偏移后的坐标
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		// 创建拾取坐标使用的起始点和最终位置
		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y,Pt->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y,Pt->GetComponentLocation().Z - 500.f);

		// 获取到周围过滤的对象
		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			IgnoreActors,
			TArray<AActor*>(),
			1500.f,
			GetActorLocation());

		// 创建一条直线来拾取地面的坐标
		FHitResult HitResult; // 存储碰撞检测结果
		FCollisionQueryParams QueryParams; // 碰撞查询参数配置
		QueryParams.AddIgnoredActors(IgnoreActors); // 设置需要忽略的Actor列表（排除玩家）

		// 使用预设的碰撞配置文件"BlockAll"进行单次线段碰撞检测
		GetWorld()->LineTraceSingleByProfile(
			HitResult,		// 存储碰撞结果
			RaisedLocation,		// 线段起点（高处位置）
			LoweredLocation,	// 线段终点（低处位置）
			FName("BlockAll"),	// 碰撞配置文件名称
			QueryParams			// 碰撞参数配置
		);

		// 通过碰撞结果调整节点的位置和朝向
		const FVector AdjustedLocation = FVector(
			Pt->GetComponentLocation().X,	// 保持X坐标不变
			Pt->GetComponentLocation().Y,	// 保持Y坐标不变
			HitResult.ImpactPoint.Z			// 使用碰撞点的Z坐标（地面高度）
		);
		Pt->SetWorldLocation(AdjustedLocation); // 设置新的世界坐标位置

		// 根据碰撞法线方向设置组件朝向（使组件正面朝向地面法线方向）
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal)); // 通过法线方向生成旋转

		//添加到返回数组内
		ArrayCopy.Add(Pt);
	}

	return ArrayCopy;
}

