// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UElectrocute : public UAuraBeamSpell
{
	GENERATED_BODY()
public:
	virtual FString GetDescription_Implementation(int32 Level) override;
	virtual FString GetNextLevelDescription_Implementation(int32 Level) override;

	/**
	 * 当前技能的技能描述，将共用部分抽离出来为单个函数
	 * @param Level 显示的技能等级
	 * @param Title 技能标题
	 * @return 技能描述文本
	 */
	virtual FString GetDescriptionAtLevel(int32 Level, const FString& Title) override;
	

};
