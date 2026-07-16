// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Data/LevelUpInfo.h"


int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		// 索引[0]为空数据，索引下标表示对应等级的数据，如果遍历完成了整个数组，说明当前等级已经达到最大值，直接返回当前等级
		if (LevelUpInformation.Num() - 1 <= Level) return Level;
		// 判断当前经验值是否已达到对应的等级所需的经验值
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			// 如果经验值达到,等级+1并继续遍历下一个等级
			++Level;
		}
		else
		{
			//如果经验值没有达到，降停止循环
			bSearching = false;
		}
	}
	return Level;
}
