// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& AttributeInfo : AttributeInformation)
	{
		//判断标签是否完全匹配
		if (AttributeInfo.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return AttributeInfo;
		}
	}

	//未查询到是否打印
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("无法从数据列表[%s]中查找到对应的AttributeTag[%s]"), *GetNameSafe(this), *AttributeTag.ToString());
	}

	return FAuraAttributeInfo();
}
