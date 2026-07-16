# Aura 项目 Wiki

> 俯视角 ARPG · UE5.4 · Gameplay Ability System（GAS）
> 本 Wiki 基于源码逐系统梳理，所有类名、函数名、路径均已核对真实代码。

---

## 目录

- [1. 项目概述](#1-项目概述)
- [2. 模块与目录结构](#2-模块与目录结构)
- [3. GameplayTag 单例](#3-gameplaytag-单例)
- [4. 属性系统](#4-属性系统)
- [5. 伤害与 Debuff 管线](#5-伤害与-debuff-管线)
- [6. 技能系统](#6-技能系统)
- [7. 输入驱动技能激活](#7-输入驱动技能激活)
- [8. 角色体系](#8-角色体系)
- [9. AI 系统](#9-ai-系统)
- [10. UI / MVC 架构](#10-ui--mvc-架构)
- [11. 数据驱动配置](#11-数据驱动配置)
- [12. 存档与关卡切换](#12-存档与关卡切换)
- [13. 战斗表现层](#13-战斗表现层)
- [14. 关键代码路径速查表](#14-关键代码路径速查表)

---

## 1. 项目概述

| 字段 | 内容 |
|---|---|
| 引擎 | Unreal Engine **5.4** |
| 类型 | 俯视角 Action-RPG，支持单人 + 本地联机 |
| 核心框架 | Gameplay Ability System（GAS） |
| 开发周期 | 2025/01 — 2025/03 |
| 规模 | 10+ 技能（含 3 条被动）、12 种属性、4 条抗性、5 种 Debuff |
| 架构关键词 | GAS · MVC · 数据驱动 · GameplayTag 状态机 · 接口解耦 |

整个项目的中心思路只有三点：
1. **属性、伤害、状态全部走 GAS**，UI 从不直接读游戏状态；
2. **C++ 做底层逻辑，蓝图做配置与表现**，通过 `BlueprintImplementableEvent` / `BlueprintNativeEvent` 衔接；
3. **所有策划数值收敛到 DataAsset**（Ability/Attribute/LevelUp/Loot/CharacterClass Info），C++ 只写访问逻辑。

---

## 2. 模块与目录结构

`Source/Aura/` 一个主模块，Public/Private 严格镜像。

```
Source/Aura/Public/
├── AbilitySystem/                     GAS 所有子系统
│   ├── AuraAbilitySystemComponent.h   ASC（挂在 PlayerState 或 Enemy 自身）
│   ├── AuraAttributeSet.h             20+ 属性 + 2 个 Meta
│   ├── AuraAbilitySystemLibrary.h     蓝图函数库（工具方法）
│   ├── AuraAbilityTypes.h             FDamageEffectParams / EffectContext 扩展
│   ├── Abilities/                     GameplayAbility 各子类
│   ├── AbilityTasks/                  自定义 AbilityTask
│   ├── AsyncTasks/                    WaitCooldownChange 等蓝图异步节点
│   ├── ExecCalc/                      ExecCalc_Damage（伤害计算）
│   ├── ModMagCalc/                    MMC_MaxHealth / MMC_MaxMana
│   ├── Debuff/                        DebuffNiagaraComponent（VFX 子组件）
│   ├── Passive/                       PassiveNiagaraComponent
│   └── Data/                          各类配置 DataAsset
├── Character/
│   ├── AuraCharacterBase.h            抽象基类（ASC/AS 容器、死亡、Debuff 状态位）
│   ├── AuraCharacter.h                玩家角色（ASC 走 PlayerState）
│   └── AuraEnemy.h                    敌人（ASC 自持 + 行为树）
├── Player/
│   ├── AuraPlayerState.h              玩家 ASC 宿主，Level/XP/Points
│   └── AuraPlayerController.h         输入转发、鼠标点击移动、伤害飘字 RPC
├── AI/                                AIController + BTService/BTTask
├── UI/
│   ├── HUD/                           AuraHUD：三个 WidgetController 总装点
│   ├── Widget/                        AuraUserWidget、DamageTextComponent
│   ├── WidgetController/              MVC 的 C（Overlay/AttrMenu/SpellMenu）
│   └── ViewModel/                     MVVM_LoadScreen / MVVM_LoadSlot（存档 UI）
├── Input/                             AuraInputConfig + AuraInputComponent
├── Interaction/                       CombatInterface / PlayerInterface / EnemyInterface 等
├── Game/                              AuraGameModeBase / AuraGameInstance / SaveGame
├── Actor/                             AuraProjectile / AuraFireBall / AuraEffectActor ...
├── Checkpoint/                        Checkpoint + MapEntrance（关卡入口）
├── AuraGameplayTags.h                 FAuraGameplayTags 静态单例 + 映射表
├── AuraAssetManager.h                 启动时调用 InitNativeGameplayTags
└── Aura.h                             模块头
```

---

## 3. GameplayTag 单例

**文件**：`Source/Aura/Public/AuraGameplayTags.h`
**访问**：`FAuraGameplayTags::Get()`（全局唯一实例）
**注册**：`UAuraAssetManager::StartInitialLoading()` → `FAuraGameplayTags::InitializeNativeGameplayTags()`，通过 `UGameplayTagsManager::Get().AddNativeGameplayTag()` 在引擎启动阶段注册，避免运行期字符串查表。

Tag 分族：

| 族 | 例 | 用途 |
|---|---|---|
| Attributes_Primary | `Strength / Intelligence / Resilience / Vigor` | 主属性，升级加点面板 |
| Attributes_Secondary | `Armor / BlockChance / CriticalHitChance ...` | 派生属性，MMC 动态算 |
| Attributes_Resistance | `Fire / Lightning / Arcane / Physical` | ECC 中折减伤害 |
| Attributes_Meta | `IncomingDamage / IncomingXP` | 只是"传递值"的临时属性 |
| InputTag | `InputTag.LMB / RMB / 1 / 2 / Passive_1` | 绑定输入到 Ability |
| Damage | `Damage.Fire / Lightning / Arcane / Physical` | 标识本次伤害类型 |
| Debuff | `Debuff.Burn / Stun / Arcane / Physical` | 对应伤害类型的异常状态 |
| Abilities_Status | `Locked / Eligible / Unlocked / Equipped` | 技能解锁 + 装备状态机 |
| Abilities_Type | `Offensive / Passive / None` | 区分主动/被动 |

**两张关键映射**（写在 `FAuraGameplayTags` 成员里）：
- `DamageTypesToResistances` — Damage.Fire → Attributes_Resistance_Fire
- `DamageTypesToDebuffs` — Damage.Fire → Debuff.Burn

在 `ExecCalc_Damage::Execute_Implementation` 里通过这两张表做一次循环就能同时算完"4 种伤害 × 4 种抗性 × 4 种 Debuff"，避免写 switch-case。

---

## 4. 属性系统

### 4.1 声明

`UAuraAttributeSet`（`Source/Aura/Public/AbilitySystem/AuraAttributeSet.h`）继承自 `UAttributeSet`。
每个属性用 `FGameplayAttributeData` + `ReplicatedUsing=OnRep_*` + `ATTRIBUTE_ACCESSORS` 宏三件套：

```cpp
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
FGameplayAttributeData Strength;
ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);
```

`ATTRIBUTE_ACCESSORS` 自动展开出 `Get/Set/Init/PropertyGetter` 四组静态函数，让蓝图/C++可以直接 `UAuraAttributeSet::GetStrengthAttribute()` 拿到 FGameplayAttribute。

### 4.2 分层

| 层 | 属性 | 数量 | 来源 |
|---|---|---|---|
| Primary | Strength / Intelligence / Resilience / Vigor | 4 | 升级加点（InfiniteGE） |
| Secondary | Armor / ArmorPenetration / BlockChance / CriticalHitChance/Damage/Resistance / HealthRegeneration / ManaRegeneration / MaxHealth / MaxMana | 12 | 由 Primary 通过 **MMC** 动态算 |
| Resistance | FireResistance / LightningResistance / ArcaneResistance / PhysicalResistance | 4 | 初始化 GE |
| Vital | Health / Mana | 2 | 战斗中实时变化 |
| Meta | IncomingDamage / IncomingXP | 2 | "中转属性"，不复制 |

### 4.3 三个关键回调

| 回调 | 触发点 | 用法 |
|---|---|---|
| `PreAttributeChange(Attribute, NewValue)` | 属性值真正修改**前** | Clamp：Health ∈ [0, MaxHealth]，Mana ∈ [0, MaxMana] |
| `PostGameplayEffectExecute(Data)` | Instant / Periodic GE 执行完**后** | 把 `IncomingDamage` 转成真实 `Health` 扣除、触发 Debuff、发送 XP 事件、飘字 |
| `PostAttributeChange(Attr, Old, New)` | 任意属性变更后 | MaxHealth 增加时自动顶满 Health（`bTopOffHealth` 标志位） |

Meta 属性 `IncomingDamage` 的用法是 GAS 典型套路：**ECC 不直接改 Health，而是把计算出的伤害写入 IncomingDamage**，由 `PostGameplayEffectExecute` → `HandleIncomingDamage` 统一处理"扣血→判定死亡→飘字→Debuff→XP→Knockback"，所有副作用在同一个函数里原子化。

### 4.4 MMC — ModMagnitudeCalculation

**位置**：`Source/Aura/Public/AbilitySystem/ModMagCalc/MMC_MaxHealth.h` / `MMC_MaxMana.h`

`MaxHealth` 为什么不是策划配的定值？因为它要随 Vigor 和等级变。`UMMC_MaxHealth` 继承 `UGameplayModMagnitudeCalculation`：

1. 构造函数里声明一个 `FGameplayEffectAttributeCaptureDefinition VigorDef`（`Source`, `Snapshot=false`）；
2. 在 `GetCapturedAttributes()` 里登记 VigorDef；
3. 重写 `CalculateBaseMagnitude_Implementation()`：`GetCapturedAttributeMagnitude(VigorDef, Spec, Eval, Vigor)` 拿到当前 Vigor，结合等级做线性公式。

最终在一条 Infinite GE（`GE_SecondaryAttributes`）里把 MMC 挂到 MaxHealth Modifier 的 Magnitude 字段上——Vigor 变化时 MaxHealth 自动重算，不用写一行 Tick。

MaxMana 同理，捕获 Intelligence。

---

## 5. 伤害与 Debuff 管线

### 5.1 ExecCalc_Damage

**位置**：`Source/Aura/Public/AbilitySystem/ExecCalc/ExecCalc_Damage.h`，`.cpp` 里实现。
继承自 `UGameplayEffectExecutionCalculation`，重写 `Execute_Implementation`。

**六步流程**：

1. **捕获属性**：Armor、ArmorPenetration、BlockChance、CriticalHitChance/Damage/Resistance、4 条元素抗性 —— 全部用 `FGameplayEffectAttributeCaptureDefinition` 静态结构预声明。
2. **取双方等级**：`ICombatInterface::Execute_GetPlayerLevel()` 分别取 Source/Target（等级影响系数查表）。
3. **聚合各元素伤害**：遍历 `DamageTypesToResistances`，对每种伤害 `D_elem = SetByCallerMagnitude(Damage.Elem) × (100 - TargetResistance_elem) / 100` 求和。
4. **格挡判定**：随机数 < TargetBlockChance 时伤害减半。
5. **护甲折减**：`EffectiveArmor = TargetArmor × (100 - SourceArmorPenetration × APCoef) / 100`，再按曲线表换算成减免率。
6. **暴击**：`EffectiveCrit = SourceCritChance × (100 - TargetCritResistance) / 100`，命中则 `Damage × 2 + CriticalHitDamage`。
7. 结果写入 `OutExecutionOutput` 的 `IncomingDamage` —— 不是 Health！

### 5.2 Debuff

**在同一个 ExecCalc 里**通过 `DetermineDebuff()` 顺带算：

```
EffectiveDebuffChance = Source.DebuffChance × (100 - Target.DebuffResistance) / 100
if (rand < EffectiveDebuffChance)
    在 EffectContext 上挂 {DamageType, DebuffDamage, DebuffDuration, DebuffFrequency}
```

> 注意 Debuff 的 **Damage/Duration/Frequency** 都是通过 `SetByCaller` 从 GameplayAbility 传进来的，没有硬编码。

真正的 DoT 逻辑由 `UAuraAttributeSet::Debuff()`（AttributeSet 内部私有方法）在 `PostGameplayEffectExecute` 里被调用，动态创建一个 Duration/Periodic GE 套到目标身上。这样 Debuff 本身也是 GE，支持叠加、刷新、Tag 中断。

### 5.3 FDamageEffectParams

**位置**：`Source/Aura/Public/AbilitySystem/AuraAbilityTypes.h`
这是 Ability → GE 之间传参的"合同"：

```cpp
FGameplayEffectContextHandle  ContextHandle;
float                         BaseDamage;
FGameplayTag                  DamageType;
float                         DebuffChance;
float                         DebuffDamage;
float                         DebuffDuration;
float                         DebuffFrequency;
float                         DeathImpulseMagnitude;
float                         KnockbackForceMagnitude;
bool                          bIsRadialDamage;
...
```

统一打包传给 `UAuraAbilitySystemLibrary::ApplyDamageEffect`，方便扩展（火球想加击退，加一个字段就行）。

---

## 6. 技能系统

### 6.1 继承关系

```
UGameplayAbility
└─ UAuraGameplayAbility                      基类（InputTag/Cost/Cooldown/Description）
   ├─ UAuraDamageGameplayAbility             带伤害字段（TMap<Tag, ScalableFloat> DamageTypes）
   │  ├─ AuraProjectileSpell                 弹体：SpawnProjectile(NumProjectiles, Pitch)
   │  │  └─ AuraFireBolt                     扇形散射
   │  ├─ AuraBeamSpell                       射线：StoreMouseDataInfo → TraceFirstTarget → StoreAdditionalTargets
   │  │  └─ Electrocute                      链式闪电（MaxNumShockTargets=5）
   │  ├─ AuraFireBlast                       召唤 FireBall 环绕飞出
   │  ├─ ArcaneShards                        地面 AOE
   │  └─ AuraMeleeAttack                     普攻（蒙太奇 + 武器 Socket 伤害）
   ├─ AuraPassiveAbility                     被动（HaloOfProtection / LifeSiphon / ManaSiphon）
   └─ AuraSummonAbility                      召唤物
```

### 6.2 学习/装备状态机

每个技能在玩家身上有一个 `FGameplayAbilitySpec`，其 **DynamicAbilityTags** 里挂两个 Tag：`Abilities_Status_*` 和 `InputTag_*`（技能槽）。

```
[不可用] Locked
   │  等级达标（ULevelUpInfo::LevelUpRequirement 表）
   ▼
[可学习] Eligible
   │  点 SpellPoint 花费 1 点（ServerSpendSpellPoint）
   ▼
[已解锁] Unlocked
   │  拖到技能槽（ServerEquipAbility）
   ▼
[已装备] Equipped
```

所有状态迁移的 **Server RPC 入口** 都在 `UAuraAbilitySystemComponent`：
- `ServerUpgradeAttribute(AttributeTag)` — 加属性点
- `ServerSpendSpellPoint(AbilityTag)` — 升级技能
- `ServerEquipAbility(AbilityTag, Slot)` — 装备技能到某个 InputTag 槽
- `ClientEquipAbility(...)` — 服务器通知发起客户端刷新 UI
- `ClientUpdateAbilityStatus(...)` — 服务器通知单个技能状态变化
- `UpdateAbilityStatuses(Level)` — 升级时批量把 Locked 推到 Eligible

### 6.3 技能描述的蓝图/C++ 桥接

```cpp
// AuraGameplayAbility.h
virtual FString GetDescription(int32 Level);
virtual FString GetNextLevelDescription(int32 Level);
```

每个具体技能类 override 这俩函数，拼一串 RichText（颜色 + 数值）。SpellMenu UI 直接绑定，学完一级就立刻能看到下一级描述。

---

## 7. 输入驱动技能激活

这是 **GAS 里最容易掉 bug 的一环**，本项目用三件套彻底数据化：

### 7.1 DataAsset：输入→Tag 映射

**`UAuraInputConfig`**（`Source/Aura/Public/Input/AuraInputConfig.h`）

```cpp
USTRUCT()
struct FAuraInputAction
{
    UPROPERTY(EditDefaultsOnly) const UInputAction* InputAction = nullptr;
    UPROPERTY(EditDefaultsOnly) FGameplayTag InputTag;
};

UCLASS()
class UAuraInputConfig : public UDataAsset
{
    TArray<FAuraInputAction> AbilityInputActions;
    const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& Tag, bool bLogNotFound) const;
};
```

策划在 DA_InputConfig 里配 [IA_LMB → InputTag.LMB]，完全不动代码。

### 7.2 Component：模板化批量绑定

**`UAuraInputComponent`**（继承 `UEnhancedInputComponent`）：

```cpp
template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void BindAbilityActions(const UAuraInputConfig* InputConfig,
                        UserClass* Object,
                        PressedFuncType PressedFunc,
                        ReleasedFuncType ReleasedFunc,
                        HeldFuncType HeldFunc);
```

在 `AAuraPlayerController::SetupInputComponent` 里一调，所有 Ability 的三事件（Started/Completed/Triggered）就自动转发给控制器上的 `AbilityInputTagPressed/Released/Held`，参数是绑定好的 InputTag。

### 7.3 ASC：Tag 匹配激活

```cpp
void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
    for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
            AbilitySpecInputPressed(Spec);
}
```

Held / Released 同款。至此，"按键 → Tag → 激活某个技能"全流程零硬编码，换皮、改键全靠 DataAsset。

---

## 8. 角色体系

### 8.1 三级继承

```
ACharacter
└─ AAuraCharacterBase (abstract)   实现 IAbilitySystemInterface, ICombatInterface
   ├─ AAuraCharacter               (Player)   + IPlayerInterface
   └─ AAuraEnemy                   (Enemy)    + IEnemyInterface, IHighlightInterface
```

### 8.2 ASC 挂载位置对比（⚠ 面试常问）

| | Player (AAuraCharacter) | Enemy (AAuraEnemy) |
|---|---|---|
| **ASC 宿主** | `AAuraPlayerState::AbilitySystemComponent` | 自己（在 Base 里 `CreateDefaultSubobject`） |
| **ReplicationMode** | `Mixed`（GE 只复制给 OwnerOnly，GameplayCue 所有人） | `Minimal`（只复制给所有客户端必要信息） |
| **Server 初始化入口** | `PossessedBy(Controller)` | `BeginPlay()` |
| **Client 初始化入口** | `OnRep_PlayerState()` | `BeginPlay()`（Simulated） |
| **为什么不一样** | PlayerState 在 Seamless Travel 时保活，能跨关卡带状态；敌人活不过一次战斗，放 PlayerState 反而麻烦 | 无需保存、无 UI Binding 成本 |

### 8.3 InitAbilityActorInfo 两次调用

`UAbilitySystemComponent::InitAbilityActorInfo(Owner, Avatar)` 必须在 Server 和 Client 各调一次。漏调会导致：
- 只调 Server：**客户端 UI 收不到属性变化**（因为 ASC 的 AvatarActor 是 nullptr，委托绑不上）。
- 只调 Client：**服务器 Ability 激活能成功，但 CheckCost/Cooldown 可能访问空指针**。

Player 流程：
```
Server:   PossessedBy → AAuraCharacter::InitAbilityActorInfo()
          → PS->GetASC()->InitAbilityActorInfo(PS, this)
          → 服务端 ApplyDefaultAttributes + AddCharacterAbilities

Client:   OnRep_PlayerState → 同一个 InitAbilityActorInfo()
          → 客户端 Bind UI Callbacks、触发 OnAscRegistered 广播
```

Enemy 流程更简单，Server/Client 都在 BeginPlay 调一次（自持 ASC，不需要等 PlayerState 复制）。

### 8.4 Debuff 状态位同步

Base 类里：
```cpp
UPROPERTY(ReplicatedUsing=OnRep_Stunned) bool bIsStunned;
UPROPERTY(ReplicatedUsing=OnRep_Burned)  bool bIsBurned;
UPROPERTY(Replicated)                    bool bIsBeingShocked;
```

OnRep 回调里播/停 VFX、冻结移动。被 `StunTagChanged` / `HitReactTagChanged`（绑定在 ASC 的 `RegisterGameplayTagEvent`）驱动。

---

## 9. AI 系统

**AIController**：`AAuraAIController`（`Source/Aura/Public/AI/AuraAIController.h`）
- 持有 `UBehaviorTreeComponent` + `UBlackboardComponent`（基类提供）
- `OnPossess` 时 RunBehaviorTree(`AAuraEnemy::BehaviorTree`)

**BTService_FindNearestPlayer**
- 每隔 Interval 秒在 Tick 时枚举所有 Player，挑最近的写进黑板键 `TargetToFollow`
- 触发条件是敌人的"战斗 range"，配合行为树 Selector 分支

**BTTask_Attack**
- 继承 `UBTTaskNode`，`ExecuteTask` 里 `ICombatInterface::Execute_GetAttackMontages()` 挑蒙太奇
- 再通过 `UAbilitySystemBlueprintLibrary::SendGameplayEventToActor()` 触发一个 `Event.Montage.SpawnProjectile` 类型的 Ability
- 避免 AI 直接调具体 C++ 函数，全部走 GAS 事件

黑板常用键：`TargetToFollow`（AActor*）、`MoveToLocation`（Vector，巡逻点）、`WhatClass`（角色职业，供行为树分支选择不同攻击组合）。

---

## 10. UI / MVC 架构

### 10.1 核心思想

**Widget 不直接引用 ASC/AttributeSet**，只认 WidgetController。
- **M**odel — ASC + AttributeSet（游戏状态）
- **V**iew — UMG Widget（蓝图配置 + C++ UAuraUserWidget 基类）
- **C**ontroller — WidgetController（C++，属性 → UI 事件的桥）

好处：
1. UI 可以先测（给 Controller Mock 数据）；
2. ASC 的改动不会引起一堆 Widget 连锁修改；
3. 每个 Widget 可独立装配、解耦，蓝图策划看得懂。

### 10.2 基类

**`UAuraWidgetController`**（`Source/Aura/Public/UI/WidgetController/AuraWidgetController.h`）

```cpp
struct FWidgetControllerParams { APlayerController*, APlayerState*, UAbilitySystemComponent*, UAttributeSet*; };

UCLASS(BlueprintType, Blueprintable)
class UAuraWidgetController : public UObject
{
    UFUNCTION(BlueprintCallable) void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
    UFUNCTION(BlueprintCallable) virtual void BroadcastInitialValues();     // ① 打开 UI 时推一次初始值
    virtual void BindCallbacksToDependencies();                             // ② 订阅属性变化委托
};
```

为什么要拆两个函数？打开 UI 的瞬间，已经应用过的 GE 不会重放一遍 —— 如果只做 ②，Health Bar 会停在 0 直到玩家下一次受击。所以 ① 先推当前值，② 绑订阅，顺序不能反。

### 10.3 三个具体 Controller

| Controller | 关心的数据 | 关键委托 |
|---|---|---|
| `UOverlayWidgetController` | Health/MaxHealth/Mana/MaxMana/XP/Level、提示消息 | `OnHealthChanged` `OnXPPercentChangedDelegate` `MessageWidgetRowDelegate` |
| `UAttributeMenuWidgetController` | 所有 Primary/Secondary/Resistance、AttributePoint | `AttributeInfoDelegate(FAuraAttributeInfo)` |
| `USpellMenuWidgetController` | Ability 状态、SpellPoint、装备槽 | `SpellGlobeSelectedDelegate` `WaitForEquipDelegate` `SpellGlobeReassignedDelegate` |

属性变化的订阅写法（典型一行）：

```cpp
AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute())
    .AddLambda([this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); });
```

### 10.4 HUD 总装

**`AAuraHUD`** 存三个 WidgetController 单例引用，提供 `GetOverlayWidgetController(Params)` 等懒加载入口 —— 谁第一次来取就 `NewObject` + `SetWidgetControllerParams` + `BindCallbacksToDependencies` + `BroadcastInitialValues`。

### 10.5 MVVM 存档界面

存档/读档界面用的不是 MVC 而是 **UE5 MVVM**（`UE 内置 ModelViewViewModel` 插件）：
- `UMVVM_LoadSlot`（每个存档槽的 ViewModel，`FIELDNOTIFY` 属性：`PlayerName / MapName / PlayerLevel / LoadSlotName`）
- `UMVVM_LoadScreen`（三槽列表的总 ViewModel，委托 `SetWidgetSwitcherIndex`）

为什么 HUD 用 MVC、存档界面用 MVVM？
- HUD 值频繁变、绑定复杂（不止一个来源，不止一种表现），MVC 的"手动绑定"更灵活；
- 存档界面是纯表单，FieldNotify 双向绑定最省代码，UE5 MVVM 的蓝图节点写起来比手写委托快 3 倍。

---

## 11. 数据驱动配置

`Source/Aura/Public/AbilitySystem/Data/`

| 类 | 数据结构 | 作用 |
|---|---|---|
| `UCharacterClassInfo` | `TMap<ECharacterClass, FCharacterClassDefaultInfo>` + CommonGE + `UCurveTable DamageCalculationCoefficients` | 每个职业的初始 GE/技能 + 公共数据表（护甲换算曲线等） |
| `UAbilityInfo` | `TArray<FAuraAbilityInfo>`（AbilityTag / InputTag / StatusTag / CooldownTag / Icon / BackgroundMaterial / LevelRequirement / AbilityType / AbilityClass） | 技能面板的全部"外壳信息" |
| `UAttributeInfo` | `TArray<FAuraAttributeInfo>`（AttributeTag / Name / Description / Value） | 属性菜单的本地化文案 |
| `ULevelUpInfo` | `TArray<FAuraLevelUpInfo>`（LevelUpRequirement / AttributePointAward / SpellPointAward） + `FindLevelForXP(int32 XP)` | 经验曲线 |
| `ULootTiers` | `TArray<FLootItem>`（Class / Chance / MaxNum / bLootLevelOverride） | 敌人掉落表 |

"为什么 AbilityInfo 不直接挂到 Ability 默认值上？" —— 因为 UI 在 Ability 还没 Grant 时就要显示（比如技能菜单灰色格子）。从 CDO 里取太耦合，从 DataAsset 取 + Tag 索引，蓝图里一个 `FindAbilityInfoForTag(Tag)` 节点就搞定。

---

## 12. 存档与关卡切换

### 12.1 存档数据

**`ULoadScreenSaveGame`**（`Source/Aura/Public/Game/LoadScreenSaveGame.h`）继承自 `USaveGame`：

```
- SlotName / SlotIndex / SaveSlotStatus(Vacant/EnterName/Taken)
- PlayerName / PlayerLevel / XP / SpellPoints / AttributePoints / 4 Primary
- MapName / MapAssetName / PlayerStartTag / bFirstTimeLoadIn
- TArray<FSavedAbility>     (AbilityTag / Status / Slot / Type / Level)
- TArray<FSavedMap>         (MapAssetName / TArray<FSavedActor>)
    - FSavedActor: Name / Transform / TArray<uint8> Bytes（UE Archive 序列化）
```

### 12.2 Save/Load 主流程

**`AAuraGameModeBase`**：
- `SaveInGameProgressData(SaveObject)` —— 把当前玩家状态和世界状态序列化到一个 Slot
- `SaveWorldState(World, DestinationMapAssetName)` —— 遍历所有实现 `ISaveInterface` 的 Actor，把它们的 Transform + UPROPERTY(SaveGame) 序列化进 `FSavedMap`
- `LoadWorldState(World)` —— 反向还原
- `TravelToMap(SlotData)` —— 加载目标关卡并带上 SlotData

### 12.3 ISaveInterface

**`Source/Aura/Public/Interaction/SaveInterface.h`**：

```cpp
UFUNCTION(BlueprintNativeEvent) bool ShouldLoadTransform();
UFUNCTION(BlueprintNativeEvent) void LoadActor();
```

第一个让每个 Actor 自己决定"我要不要被位置还原"（例：Door 要、Enemy 不要），第二个是蓝图/C++ 各自可扩展的 hook。比起 UE 自带的 `FArchive` 序列化整个 Level 更可控，也绕开了"蓝图类无法全量序列化"的坑。

### 12.4 Checkpoint + MapEntrance

- `ACheckpoint` — PlayerStart 的增强版，`bReached`（SaveGame UPROPERTY）+ 高亮 + Sphere Overlap 广播
- `AMapEntrance : ACheckpoint` — 额外含 `TSoftObjectPtr<UWorld> DestinationMap` + `DestinationPlayerStartTag`，玩家踩上去触发 `TravelToMap`

GameMode 的 `ChoosePlayerStart_Implementation` 会读取 SaveGame 里的 `PlayerStartTag`，让玩家从上次存档的 Checkpoint 重生。

---

## 13. 战斗表现层

### 13.1 伤害飘字（Client RPC，不用 Multicast）

**`AAuraPlayerController::ShowDamageNumber_Client(float Damage, AActor* Target, bBlocked, bCritical)`**（`Client, Reliable`）

为什么不用 `MulticastHandleDeath` 那种 Multicast？**飘字只该给造成这次伤害的那个玩家看**，其他联机玩家的屏幕不该刷出全场所有伤害。服务器在 `PostGameplayEffectExecute` 里根据 SourceController 精准 Client RPC。

`UDamageTextComponent`（WidgetComponent 子类）在 RPC 里动态 `NewObject` + `AttachTo(Target)` + `SetDamageText`，自带上飘 + 淡出蓝图动画。

### 13.2 GameplayCue / Niagara 子组件

项目没用标准的 `UGameplayCueNotify_*` 资产，而是**两个常驻子组件订阅 ASC 委托**：

| 组件 | 订阅 | 效果 |
|---|---|---|
| `UDebuffNiagaraComponent`（角色身上两份：Burn/Stun） | `RegisterGameplayTagEvent(Debuff.Burn, NewOrRemoved)` | Tag 计数 >0 时激活 Niagara，=0 时停 |
| `UPassiveNiagaraComponent`（三份：HaloOfProtection/LifeSiphon/ManaSiphon） | `UAuraAbilitySystemComponent::ActivatePassiveEffect` 多播委托 | 玩家装备/卸下被动时开关特效 |

**取舍**：标准 GameplayCue 更符合 GAS 规范、可蓝图配置强；自写组件的优势是**可以直接访问 Owner，拿到武器 Socket 等**，对"长期激活的身上 VFX"更直接，初学者更易维护。

### 13.3 死亡流程

`AAuraCharacterBase::Die(FVector DeathImpulse)` →
- `MulticastHandleDeath(DeathImpulse)`（Multicast Reliable）：
  - `Weapon->SetSimulatePhysics(true)` + 施加 Impulse
  - Mesh → Ragdoll + Collision Profile 切 `Ragdoll`
  - `Dissolve()` 创建动态材质实例 → 调用蓝图 `StartDissolveTimeline` / `StartWeaponDissolveTimeline` 播曲线
  - 播放 `DeathSound`
- Player：启动 5s 定时器 → UI 打开存档/读档界面
- Enemy：`SetLifeSpan(5.f)` + `SpawnLoot()`（蓝图实现：查 LootTiers 概率生成 AuraEffectActor）

---

## 14. 关键代码路径速查表

### 14.1 GAS 核心

| 概念 | 文件 |
|---|---|
| ASC（扩展） | `Source/Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h` |
| AttributeSet（20+ 属性） | `Source/Aura/Public/AbilitySystem/AuraAttributeSet.h` |
| 蓝图函数库 | `Source/Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h` |
| Effect 自定义类型 | `Source/Aura/Public/AbilitySystem/AuraAbilityTypes.h` |

### 14.2 Ability

| 概念 | 文件 |
|---|---|
| Ability 基类 | `Source/Aura/Public/AbilitySystem/Abilities/AuraGameplayAbility.h` |
| 伤害 Ability | `Source/Aura/Public/AbilitySystem/Abilities/AuraDamageGameplayAbility.h` |
| 弹体 | `Source/Aura/Public/AbilitySystem/Abilities/AuraProjectileSpell.h` |
| 射线/链式 | `Source/Aura/Public/AbilitySystem/Abilities/AuraBeamSpell.h` |
| 被动 | `Source/Aura/Public/AbilitySystem/Abilities/AuraPassiveAbility.h` |
| 具体技能 | `FireBolt.h` / `Electrocute.h` / `FireBlast.h` / `ArcaneShards.h` / `MeleeAttack.h` |

### 14.3 计算

| 概念 | 文件 |
|---|---|
| 伤害 ECC | `Source/Aura/Public/AbilitySystem/ExecCalc/ExecCalc_Damage.h` |
| MaxHealth MMC | `Source/Aura/Public/AbilitySystem/ModMagCalc/MMC_MaxHealth.h` |
| MaxMana MMC | `Source/Aura/Public/AbilitySystem/ModMagCalc/MMC_MaxMana.h` |

### 14.4 角色 / 输入

| 概念 | 文件 |
|---|---|
| Character 基类 | `Source/Aura/Public/Character/AuraCharacterBase.h` |
| Player 角色 | `Source/Aura/Public/Character/AuraCharacter.h` |
| Enemy | `Source/Aura/Public/Character/AuraEnemy.h` |
| PlayerState | `Source/Aura/Public/Player/AuraPlayerState.h` |
| PlayerController | `Source/Aura/Public/Player/AuraPlayerController.h` |
| Input Config | `Source/Aura/Public/Input/AuraInputConfig.h` |
| Input Component | `Source/Aura/Public/Input/AuraInputComponent.h` |

### 14.5 UI

| 概念 | 文件 |
|---|---|
| WidgetController 基类 | `Source/Aura/Public/UI/WidgetController/AuraWidgetController.h` |
| HUD Overlay | `Source/Aura/Public/UI/WidgetController/OverlayWidgetController.h` |
| 属性菜单 | `Source/Aura/Public/UI/WidgetController/AttributeMenuWidgetController.h` |
| 技能菜单 | `Source/Aura/Public/UI/WidgetController/SpellMenuWidgetController.h` |
| HUD | `Source/Aura/Public/UI/HUD/AuraHUD.h` |
| MVVM 存档 | `Source/Aura/Public/UI/ViewModel/MVVM_LoadScreen.h` / `MVVM_LoadSlot.h` |

### 14.6 游戏系统

| 概念 | 文件 |
|---|---|
| GameMode | `Source/Aura/Public/Game/AuraGameModeBase.h` |
| GameInstance | `Source/Aura/Public/Game/AuraGameInstance.h` |
| SaveGame | `Source/Aura/Public/Game/LoadScreenSaveGame.h` |
| Checkpoint | `Source/Aura/Public/Checkpoint/Checkpoint.h` |
| Map Entrance | `Source/Aura/Public/Checkpoint/MapEntrance.h` |
| AssetManager | `Source/Aura/Public/AuraAssetManager.h` |
| GameplayTags | `Source/Aura/Public/AuraGameplayTags.h` |

### 14.7 数据

| 概念 | 文件 |
|---|---|
| 职业数据 | `Source/Aura/Public/AbilitySystem/Data/CharacterClassInfo.h` |
| 技能元数据 | `Source/Aura/Public/AbilitySystem/Data/AbilityInfo.h` |
| 属性元数据 | `Source/Aura/Public/AbilitySystem/Data/AttributeInfo.h` |
| 升级曲线 | `Source/Aura/Public/AbilitySystem/Data/LevelUpInfo.h` |
| 掉落表 | `Source/Aura/Public/AbilitySystem/Data/LootTiers.h` |

### 14.8 接口（Interaction/ 下全是 UInterface）

- `CombatInterface` — 战斗通用（GetPlayerLevel / Die / GetCombatSocketLocation / GetAttackMontages ...）
- `PlayerInterface` — 玩家专属（AddToXP / LevelUp / SaveProgress ...）
- `EnemyInterface` — 敌人专属（SetCombatTarget / GetCombatTarget）
- `HighlightInterface` — 可高亮物体
- `SaveInterface` — Actor 存档

---

*文档生成时间：2026-04。如有源码变更，以代码为准。*
