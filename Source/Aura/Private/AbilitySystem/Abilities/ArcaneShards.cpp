// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/Abilities/ArcaneShards.h"


FString UArcaneShards::GetDescription_Implementation(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"奥术爆发");
}

FString UArcaneShards::GetNextLevelDescription_Implementation(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"下一等级");
}

FString UArcaneShards::GetDescriptionAtLevel(int32 Level, const FString& Title)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// 标题
		"<Title>%s</>\n"

		// 细节
		"<Small>等级：</> <Level>%i</>\n"
		"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
		"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

		// 技能描述
		"<Default>首次激活技能使用魔法光圈划定攻击位置，再次释放在魔法光圈位置生成 %i 奥术碎片，攻击附近敌人，造成</> <Damage>%i</> <Default>点奥术伤害。</>"),

		// 动态修改值
		*Title,
		Level,
		Cooldown,
		-ManaCost,
		FMath::Min(Level, MaxNumShards),
		ScaledDamage);
}
