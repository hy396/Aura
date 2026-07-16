// Huanyu,miaomiaomiao,nekoneko


#include "MyGameplayTags.h"

#include "GameplayTagsManager.h"

FMyGameplayTags FMyGameplayTags::GameplayTags;

void FMyGameplayTags::InitializeNativeGameplayTags()
{
	/*=======================================================
	* Attribute System - Primary Attributes 属性系统-主要属性
	* 定义角色核心成长属性，影响基础战斗能力
	*=======================================================*/
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("力量值,增加物理伤害")
		);
	
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("智力值,增加魔法伤害")
		);
	
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("韧性,增加护甲和护甲穿透")
		);
	
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("体力,增加血量上限")
		);

	/*=======================================================
	* Attribute System - Secondary Attributes 属性系统-次要属性
	* 衍生属性，受主要属性影响，提供具体战斗数值
	*=======================================================*/
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("可以获得的最大生命值")
		);
	
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("可以获得的最大法力值")
		);
	
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("减少所受伤害，提高格挡几率")
		);
		
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("忽略敌方护甲百分比，增加暴击几率")
		);
	
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("有几率将受到的伤害减半")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("概率触发伤害暴击")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("暴击触发时伤害提高的倍率")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("降低攻击者的暴击几率")
		);
	
	
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("每秒自动回复的生命值")
		);
	
	
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("每秒自动回复的法力值")
		);

	/*=======================================================
	* 元属性
	*=======================================================*/
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Attributes.Meta.IncomingXP"),
			FString("经验元属性标签")
			);
	/*=======================================================
	* Input Mapping System 输入映射系统
	* 定义玩家输入与游戏动作的绑定关系
	*=======================================================*/
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("鼠标左键")
		);
	
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("鼠标右键")
		);
	
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("键盘1键")
		);
	
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("键盘2键")
		);
	
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("键盘3键")
		);
	
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("键盘4键")
		);
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.1"),
		FString("被动技能1输入标签")
		);

	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.2"),
		FString("被动技能2输入标签")
		);
	/*=======================================================
	* Damage Type System 伤害类型系统
	* 定义游戏内不同属性的伤害类型
	* DamageType -> Resistance 映射见 DamageResistanceMap
	*=======================================================*/	
	GameplayTags.Damage = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Damage"),
		FString("伤害标签")
		);
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Fire"),
			FString("火属性伤害")
		);
	
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Lightning"),
			FString("雷属性伤害")
		);
	
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Arcane"),
			FString("魔法伤害")
		);
	
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Physical"),
			FString("物理伤害")
		);
	
	/*=======================================================
	* Resistance System 抗性系统
	* 与DamageType一一对应，提供伤害减免计算
	*=======================================================*/
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Fire"),
			FString("火属性抗性")
			);
	
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Lightning"),
			FString("雷属性抗性")
			);
	
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Arcane"),
			FString("魔法伤害抗性")
			);
	
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Physical"),
			FString("物理伤害抗性")
		);

	/*=======================================================
	* DeBuff 负面标签注册
	*=======================================================*/
	GameplayTags.DeBuff_Burn = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Burn"),
			FString("火属性燃烧负面标签")
			);
	GameplayTags.DeBuff_Stun = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Stun"),
			FString("雷属性眩晕负面标签")
			);
	GameplayTags.DeBuff_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Arcane"),
			FString("魔法属性负面标签")
			);
	GameplayTags.DeBuff_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Physical"),
			FString("物理属性流血负面标签")
			);

	/*=======================================================
	* 负面效果配置标签
	*=======================================================*/
	GameplayTags.DeBuff_Chance = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Chance"),
			FString("负面效果 触发几率")
			);
	GameplayTags.DeBuff_Damage = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Damage"),
			FString("负面效果 伤害")
			);
	GameplayTags.DeBuff_Duration = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Duration"),
			FString("负面效果 持续时间")
			);
	GameplayTags.DeBuff_Frequency = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("DeBuff.Frequency"),
			FString("负面效果 触发间隔")
			);

	/*=======================================================
	* Abilities 攻击技能
	*=======================================================*/
	GameplayTags.Abilities_None = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.None"),
			FString("'空能力'标记——相当于能力标签的空指针")
			);
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Attack"),
			FString("攻击技能标签")
		);
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Summon"),
			FString("召唤技能标签")
			);
	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.HitReact"),
		FString("受击技能标签")
		);

	/*=======================================================
	* 当前技能状态标签
	*=======================================================*/
	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Locked"),
			FString("已锁定")
			);
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Eligible"),
			FString("可解锁")
			);
	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Unlocked"),
			FString("已解锁")
			);
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Equipped"),
			FString("已装配")
			);

	/*=======================================================
	* 当前技能类型标签
	*=======================================================*/
	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Type.Offensive"),
			FString("主动技能")
			);
	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Type.Passive"),
			FString("被动技能")
			);
	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Type.None"),
			FString("啥也不是")
			);


	
	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Fire.FireBolt"),
			FString("火球术技能标签")
			);
	//Abilities_Fire_FireBlast
	GameplayTags.Abilities_Fire_FireBlast = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Fire.FireBlast"),
		FString("火焰爆发技能标签")
		);
	GameplayTags.Abilities_Barrage = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Barrage"),
		FString("弹幕技能标签")
		);
	GameplayTags.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Lightning.Electrocute"),
		FString("电击技能标签")
		);
	GameplayTags.Abilities_Arcane_ArcaneShards = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Arcane.ArcaneShards"),
		FString("奥术技能奥术爆发标签")
		);

	/*=======================================================
	* 被动技能
	*=======================================================*/
	GameplayTags.Abilities_Passive_HaloOfProtection = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Passive.HaloOfProtection"),
			FString("守护光环")
			);
	GameplayTags.Abilities_Passive_LifeSiphon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Passive.LifeSiphon"),
			FString("生命自动回复")
			);
	GameplayTags.Abilities_Passive_ManaSiphon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Passive.ManaSiphon"),
			FString("蓝量自动回复")
			);

	
	//Abilities_Barrage
	/*=======================================================
	* Cooldown 冷却
	*=======================================================*/
	GameplayTags.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBolt"),
		FString("火球术冷却标签")
		);
	//Cooldown_Barrage
	GameplayTags.Cooldown_Barrage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Barrage"),
	FString("弹幕攻击冷却标签")
	);
	/*=======================================================
	* Combat Feedback System 战斗反馈系统
	*=======================================================*/
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Effects.HitReact"),
		FString("受到攻击时，赋予的标签")
		);
	

	/*=======================================================
	* CombatSocket 攻击部位标签
	*=======================================================*/
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("CombatSocket.Weapon"),
			FString("使用武器攻击部位标签")
			);
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("CombatSocket.LeftHand"),
			FString("左手攻击部位标签")
			);
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("CombatSocket.RightHand"),
			FString("右手攻击部位标签")
		);
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Tail"),
	FString("尾巴攻击部位标签")
	);
	
	/*=======================================================
	* Montage 蒙太奇动画使用攻击动作索引
	*=======================================================*/
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Montage.Attack.1"),
		FString("第1个蒙太奇攻击标签")
		);

	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.2"),
			FString("第2个蒙太奇攻击标签")
			);

	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.3"),
			FString("第3个蒙太奇攻击标签")
			);

	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.4"),
			FString("第4个蒙太奇攻击标签")
			);
	/*=======================================================
	* 阻止相关鼠标事件的触发标签
	*=======================================================*/
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.InputPressed"),
			FString("阻挡键位按下输入")
			);
	GameplayTags.Player_Block_InputHold = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.InputHold"),
			FString("阻挡键位悬停输入")
			);
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.InputReleased"),
			FString("阻挡键位抬起输入")
			);
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.CursorTrace"),
			FString("阻挡鼠标拾取事件")
			);

	
	/*=======================================================
	* GameplayCues 游戏效果标签
	*=======================================================*/
	GameplayTags.GameplayCue_FireBlast = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.FireBlast"),
		FString("火焰爆发游戏效果标签")
		);

	
	/*=======================================================
	* Damage Type Mapping System 伤害类型映射系统
	*=======================================================*/
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);

	/*=======================================================
	* DeBuff System (Commented) 减益效果系统
	*=======================================================*/
	GameplayTags.DamageTypesToDeBuffs.Add(GameplayTags.Damage_Arcane, GameplayTags.DeBuff_Arcane);
	GameplayTags.DamageTypesToDeBuffs.Add(GameplayTags.Damage_Lightning, GameplayTags.DeBuff_Stun);
	GameplayTags.DamageTypesToDeBuffs.Add(GameplayTags.Damage_Physical, GameplayTags.DeBuff_Physical);
	GameplayTags.DamageTypesToDeBuffs.Add(GameplayTags.Damage_Fire, GameplayTags.DeBuff_Burn);

	//UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("减少受到的伤害，提高格挡率"));
}



