// Huanyu,miaomiaomiao,nekoneko


#include "AI/BTTask_Attack.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory); //不删除是为了保证在蓝图中正常执行对应回调 ReceiveExecuteAI 和 ReceiveExecute
}
