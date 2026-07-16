// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * GameplayTags 标签 单例模式
 * 内部包含原生的项目中使用的标签
 */

struct FMyGameplayTags
{
public:
	static const FMyGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();
	// static void InitializeAttributeGameplayTags();
	// static void InitializeInputGameplayTags();
	


	/*=======================================================
	* Attribute System - Primary Attributes 属性系统-主要属性
	* 定义角色核心成长属性，影响基础战斗能力
	*=======================================================*/
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/*=======================================================
	* Attribute System - Secondary Attributes 属性系统-次要属性
	* 衍生属性，受主要属性影响，提供具体战斗数值
	*=======================================================*/
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;

	/*=======================================================
	* 元属性标签 元属性经验
	*=======================================================*/
	FGameplayTag Attributes_Meta_IncomingXP;
	/*=======================================================
	* Input Mapping System 输入映射系统
	* 定义玩家输入与游戏动作的绑定关系
	*=======================================================*/
	FGameplayTag InputTag_LMB; //鼠标左键
	FGameplayTag InputTag_RMB; //鼠标右键
	FGameplayTag InputTag_1; //1键
	FGameplayTag InputTag_2; //2键
	FGameplayTag InputTag_3; //3键
	FGameplayTag InputTag_4; //4键
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;
	
	/*=======================================================
	* Damage Type System 伤害类型系统
	* 定义游戏内不同属性的伤害类型
	* DamageType -> Resistance 映射见 DamageResistanceMap
	*=======================================================*/	
	FGameplayTag Damage;
	FGameplayTag Damage_Fire; //火属性伤害 
	FGameplayTag Damage_Lightning; //雷属性伤害 
	FGameplayTag Damage_Arcane; //魔法伤害
	FGameplayTag Damage_Physical; //物理伤害
	
	/*=======================================================
	* Resistance System 抗性系统
	* 与DamageType一一对应，提供伤害减免计算
	*=======================================================*/
	FGameplayTag Attributes_Resistance_Fire; //火属性伤害抵抗 
	FGameplayTag Attributes_Resistance_Lightning; //雷属性伤害抵抗 
	FGameplayTag Attributes_Resistance_Arcane; //魔法伤害抵抗 
	FGameplayTag Attributes_Resistance_Physical; //物理伤害抵抗
	
	/*=======================================================
	* DeBuff 负面效果标签
	*=======================================================*/
	//火属性负面效果 燃烧
	FGameplayTag DeBuff_Burn;
	//雷属性负面效果 眩晕
	FGameplayTag DeBuff_Stun;
	//魔法伤害负面效果
	FGameplayTag DeBuff_Arcane;
	//物理伤害负面效果 流血
	FGameplayTag DeBuff_Physical;
	
	/*=======================================================
	* DeBuff Effect 负面效果属性标签
	*=======================================================*/
	//负面效果触发几率标签
	FGameplayTag DeBuff_Chance;
	//负面效果伤害标签
	FGameplayTag DeBuff_Damage;
	//负面效果持续时间标签
	FGameplayTag DeBuff_Duration;
	//负面效果触发间隔标签
	FGameplayTag DeBuff_Frequency;
	
	/*=======================================================
	* CombatSocket 攻击部位标签
	*=======================================================*/
	//使用武器攻击部位标签
	FGameplayTag CombatSocket_Weapon;
	//右手攻击部位标签
	FGameplayTag CombatSocket_RightHand;
	//左手攻击部位标签
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;
	/*=======================================================
	* Montage 蒙太奇动画 使用攻击动作索引
	*=======================================================*/
	FGameplayTag Montage_Attack_1; 
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	
	FGameplayTag Abilities_None;
	//攻击技能激活标签
	FGameplayTag Abilities_Attack;
	//召唤技能激活标签
	FGameplayTag Abilities_Summon;
	
	//受击技能标签
	FGameplayTag Abilities_HitReact;
	
	/*=======================================================
	* 能力状态标签
	*=======================================================*/
	// 已锁定
	FGameplayTag Abilities_Status_Locked;
	// 可解锁
	FGameplayTag Abilities_Status_Eligible;
	// 已解锁
	FGameplayTag Abilities_Status_Unlocked;
	// 已装配
	FGameplayTag Abilities_Status_Equipped;
	/*=======================================================
	* 能力类型标签
	*=======================================================*/
	// 主动技能
	FGameplayTag Abilities_Type_Offensive;
	// 被动技能
	FGameplayTag Abilities_Type_Passive;
	// 空 受击登技能设置
	FGameplayTag Abilities_Type_None;
	
	/*=======================================================
	* 主动技能标签
	*=======================================================*/
	//火球术技能标签
	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Abilities_Fire_FireBlast;
	//弹幕技能标签
	FGameplayTag Abilities_Barrage;
	//雷击技能标签
	FGameplayTag Abilities_Lightning_Electrocute;
	//奥数爆发技能标签
	FGameplayTag Abilities_Arcane_ArcaneShards; 

	
	/*=======================================================
	* 被动技能标签
	*=======================================================*/
	// 被动技能-守护光环
	FGameplayTag Abilities_Passive_HaloOfProtection;
	// 被动技能-生命回复
	FGameplayTag Abilities_Passive_LifeSiphon;
	// 被动技能-蓝量回复
	FGameplayTag Abilities_Passive_ManaSiphon; 
	
	/*=======================================================
	* 技能冷却标签
	*=======================================================*/
	// 火球技能冷却标签
	FGameplayTag Cooldown_Fire_FireBolt;
	// 弹幕技能冷却标签
	FGameplayTag Cooldown_Barrage;
	// 伤害类型到抵抗类型的映射
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	// 伤害类型到负面效果的映射
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDeBuffs;
	
	// 效果标签
	FGameplayTag Effects_HitReact; // 受击
	
	//阻止输入相关事件触发
	// 阻挡键位按下输入
	FGameplayTag Player_Block_InputPressed;
	// 阻挡键位悬停输入
	FGameplayTag Player_Block_InputHold;
	// 阻挡键位抬起输入
	FGameplayTag Player_Block_InputReleased; 
	// 阻挡鼠标拾取事件
	FGameplayTag Player_Block_CursorTrace;

	
	// 游戏提示标签
	FGameplayTag GameplayCue_FireBlast;


private:
	static FMyGameplayTags GameplayTags;

	
};

