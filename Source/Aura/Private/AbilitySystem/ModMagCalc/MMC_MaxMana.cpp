// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute(); //设置需要获取的属性对象
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; //设置拾取对象为GE的应用目标
	IntDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntDef); //添加到捕获属性数值，只有添加到列表，才会去获取属性值
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//  从 source 和 target 获取 Tag
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//获取智力
	float Int = 0.f;
	GetCapturedAttributeMagnitude(IntDef, Spec, EvaluationParameters, Int);
	Int = FMath::Max<float>(Int, 0.f);

	//获取等级
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	//计算最大蓝量
	return 2333.f + 25.f * Int + 150.f * PlayerLevel;
}
