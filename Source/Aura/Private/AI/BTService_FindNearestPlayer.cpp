// Huanyu,miaomiaomiao,nekoneko


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// GEngine->AddOnScreenDebugMessage(1,1.f,FColor::Red, *AIOwner->GetName());
	// GEngine->AddOnScreenDebugMessage(2,1.f,FColor::Green, *ActorOwner->GetName());
	// 获取AI控制的Pawn对象
	APawn* OwningPawn = AIOwner->GetPawn();
	// 根据自身标签确定目标标签：如果是玩家则找敌人，反之找玩家
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	// 存储所有带目标标签的Actor
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

	// 初始化最近距离为最大值，最近角色为空
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;

	// 遍历所有目标角色
	for (AActor* Actor : ActorsWithTag)
	{
		// 计算与当前角色的距离
		const float DistanceToActor = OwningPawn->GetDistanceTo(Actor);
    
		// 如果找到更近的角色
		if (DistanceToActor < ClosestDistance)
		{
			ClosestDistance = DistanceToActor;  // 更新最近距离
			ClosestActor = Actor;               // 更新最近角色
		}
	}

	// 将找到的最近角色和距离更新到黑板数据
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);  // 设置追踪目标
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);  // 设置目标距离


}
