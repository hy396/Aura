// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	// 遍历技能信息数组
	for (const FAuraAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info; // 找到匹配的标签则返回对应技能信息
		}
	}
	// 未找到且需要记录日志时
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, 
			TEXT("未找到AbilityTag[%s]对应的技能信息，数据资源[%s]"), 
			*AbilityTag.ToString(), 
			*GetNameSafe(this));
	}

	return FAuraAbilityInfo(); // 返回空的技能信息结构体
}
