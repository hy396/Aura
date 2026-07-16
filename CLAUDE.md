# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

# Aura

俯视角 Action-RPG · **Unreal Engine 5.4** · **Gameplay Ability System (GAS)** · 支持单人 + 本地联机。

`README.md` 是项目履历/简介，`WIKI.md` 是按系统逐章节的源码对照手册（类名、文件路径已交叉核对，包含伤害管线、MMC、状态机、UI 架构等深度细节）。本文档只记录项目级"工作前提"和架构速查。

---

## Build / Run

UE5 项目，**没有** Makefile / CMakeList — 构建由 UnrealBuildTool 驱动，由编辑器或 `RunUAT.bat` 触发：

- **打开项目** — 双击 `Aura.uproject`（自动调用 UE5.4 编辑器；首次会触发 C++ 编译）。
- **生成 IDE 工程** — 编辑器打开后 `Tools → Refresh Visual Studio Project`，或在 `Source/` 目录右键 `Generate Project Files`。
- **编辑器内编译 (Live Coding)** — 编辑器运行中按 `Ctrl+Alt+F11`，C++ 改动直接热重载，无需重启。
- **完整构建** — `Aura.sln` 是已生成好的 VS 解决方案，配置 `Development Editor / Win64` 编译 `Aura` 模块即可。
- **打包** — 编辑器 `File → Package Project → Windows`。
- **测试 / 单测** — 项目**没有**独立的单元测试 target（GAS 项目惯例），逻辑验证通过 PIE Play-in-Editor 完成：编辑器 `Play → Selected Viewport / Standalone / Client` 三种模式分别测试 Server / Client / Listen Server 路径。

**第三方插件**（来自 `Plugins/`，不在 UE 市场自动拉取，依赖本地存在）：

| 插件 | 用途 |
|---|---|
| `VRM4U` | VRM 角色模型导入 |
| `KawaiiPhysics` | 物理模拟（头发/衣物） |
| `UNTLink` | Rider IDE 集成 |
| `ModelViewViewModel` | UE5 MVVM 插件（存档界面用） |

`MotionWarping` 也在 `Aura.uproject` 启用。

---

## 模块 / 目录约定

单模块项目：**`Source/Aura/`（Aura Runtime 模块）**。`Public/` 与 `Private/` 严格镜像 —— 头文件全在 `Public/`。模块依赖见 `Source/Aura/Aura.Build.cs`。

代码命名注意 —— 部分顶层头文件名沿用早期命名：

- `Source/Aura/Public/MyAssetManager.h`（继承 `UAssetManager`）
- `Source/Aura/Public/MyGameplayTags.h`（`FMyGameplayTags` 单例）
- `Source/Aura/Public/AuraAbilityTypes.h`（`FDamageEffectParams` 结构体）

`Source/Aura/Public/AbilitySystem/` 子目录（与 WIKI §2 一致）：

- `Abilities/` `AbilityTasks/` `AsyncTasks/` `ExecCalc/` `ModMagCalc/` `Data/`
- `DeBuff/` —— **大小写为 D 大写 B**，GAS 子组件
- `Passive/` — 被动技能 Niagara 子组件

`Content/` 资产：

- `Assets/` — 美术资源（SMDungeon、Characters、Mage、ShieldFX、Spells、CelesAnimeShader…）
- `Blueprints/` — 蓝图（按子系统分目录）
- `Maps/` — `MainMenu / LoadMenu / Dungeon / Dungeon_2 / Dungeon_3 / NewMap / EQS_TestingMap`

`WIKI.md` 和 `INTERVIEW.md` 是项目沉淀文档，每次源码大幅变动后需回头校对（已知与代码小差异见上）。

---

## GAS 核心架构（必读）

整个项目的中心思路（来自 WIKI §1，落地到代码时务必按此顺序思考）：

1. **属性、伤害、状态全部走 GAS**，UI 从不直接读游戏状态 —— Widget 只认 WidgetController。
2. **C++ 做底层逻辑，蓝图做配置与表现**，通过 `BlueprintImplementableEvent` / `BlueprintNativeEvent` 衔接（例：`Die` 是 `BlueprintNativeEvent`，细节蓝图实现）。
3. **所有策划数值收敛到 DataAsset**，C++ 只写访问逻辑（`Source/Aura/Public/AbilitySystem/Data/`：`CharacterClassInfo / AbilityInfo / AttributeInfo / LevelUpInfo / LootTiers`）。
4. **GameplayTag 单例** —— `FMyGameplayTags::Get()` 在 `UMyAssetManager::StartInitialLoading()` 通过 `UGameplayTagsManager::AddNativeGameplayTag()` 在引擎启动阶段注册，避免运行期字符串查表。两张关键映射 `DamageTypesToResistances` / `DamageTypesToDebuffs` 驱动 `ExecCalc_Damage` 的循环计算。

### GAS 关键路径速查

| 关注点 | 文件 |
|---|---|
| ASC（扩展 + 状态机 Server RPC） | `Source/Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h` |
| AttributeSet（20+ 属性） | `Source/Aura/Public/AbilitySystem/AuraAttributeSet.h` |
| 蓝图函数库（ApplyDamageEffect、GetClassDefaultObject 等工具） | `Source/Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h` |
| EffectContext / 打包伤害参数 | `Source/Aura/Public/AuraAbilityTypes.h` |
| 伤害 ExecCalc（含抗性/护甲/暴击/格挡/Debuff） | `Source/Aura/Public/AbilitySystem/ExecCalc/ExecCalc_Damage.h` |
| MMC（MaxHealth 捕获 Vigor / MaxMana 捕获 Intelligence） | `Source/Aura/Public/AbilitySystem/ModMagCalc/` |
| Ability 基类 / 伤害 Ability | `AbilitySystem/Abilities/AuraGameplayAbility.h`、`AuraDamageGameplayAbility.h` |
| 弹体 / 射线 / 链式 / 法球 / AOE / 普攻 / 被动 / 召唤 | `AbilitySystem/Abilities/` 子目录 |
| 自定义 AsyncTask（WaitCooldownChange 等） | `AbilitySystem/AsyncTasks/` |
| Debuff Niagara 子组件 | `AbilitySystem/DeBuff/DebuffNiagaraComponent.h` |
| Passive Niagara 子组件 | `AbilitySystem/Passive/PassiveNiagaraComponent.h` |

### 三个属性回调（`UAuraAttributeSet`）

| 回调 | 触发 | 用途 |
|---|---|---|
| `PreAttributeChange` | 属性真修改前 | Clamp（Health ∈ [0, MaxHealth]） |
| `PostGameplayEffectExecute` | Instant/Periodic GE 执行后 | `IncomingDamage → Health`、判定死亡、飘字、Debuff、XP、Knockback |
| `PostAttributeChange` | 任意属性变更后 | `bTopOffHealth` 标志位：MaxHealth 增长时自动顶满 |

Meta 属性 `IncomingDamage` 是 GAS 标准套路 —— **ECC 不直接改 Health**，由 `HandleIncomingDamage()` 统一原子化副作用。

---

## ASC 挂载位置差异（⚠ 改 Player 角色前必看）

| | Player (`AAuraCharacter`) | Enemy (`AAuraEnemy`) |
|---|---|---|
| **ASC 宿主** | `AAuraPlayerState::AbilitySystemComponent` | 自己（Base 里 `CreateDefaultSubobject`） |
| **ReplicationMode** | `Mixed` | `Minimal` |
| **Server 初始化入口** | `PossessedBy(Controller)` | `BeginPlay()` |
| **Client 初始化入口** | `OnRep_PlayerState()` | `BeginPlay()` |
| **理由** | PlayerState 在 Seamless Travel 保活，跨关卡带状态 | 敌人活不过一次战斗，不放 PlayerState |

**`InitAbilityActorInfo(Owner, Avatar)` 必须 Server / Client 各调一次**：

- Player：`PossessedBy` 与 `OnRep_PlayerState` 各调一次。
- Enemy：Server / Client 都在 `BeginPlay` 调一次。

只调 Server 会导致客户端 UI 收不到属性变化；只调 Client 会导致服务器能力激活成功但 `CheckCost/Cooldown` 访问空指针。

---

## 输入驱动技能激活（GAS 易出 bug 的一环）

三件套数据化，**零硬编码**：

1. **`UAuraInputConfig`（DataAsset）** — `TArray<FAuraInputAction>` 把 `UInputAction ↔ FGameplayTag` 配对，`FindAbilityInputActionForTag(Tag)` 反查。
2. **`UAuraInputComponent : UEnhancedInputComponent`** — 模板 `BindAbilityActions(InputConfig, Object, Pressed/Released/Held)` 批量绑定。
3. **`UAuraAbilitySystemComponent::AbilityInputTagPressed/Held/Released(const FGameplayTag&)`** — 遍历 `GetActivatableAbilities()`，对 `Spec.DynamicAbilityTags.HasTagExact(InputTag)` 的 Spec 调用 `AbilitySpecInputPressed`。

新增按键流程：`DA_InputConfig` 加一行 IA→Tag，编写输入回调，不动 C++。

---

## UI 架构

**两种架构并存** —— 不要混用：

- **HUD（Overlay/AttributeMenu/SpellMenu）用 MVC**：Widget → WidgetController → ASC/AttributeSet。Controller 基类 `UAuraWidgetController` 的两个虚函数 `BroadcastInitialValues()` 与 `BindCallbacksToDependencies()` 顺序固定（先推初值再订阅，否则 Health 会停在 0 直到下次受击），不要改顺序。
- **存档/读档界面用 UE5 MVVM**：插件 `ModelViewViewModel`。`UMVVM_LoadScreen` / `UMVVM_LoadSlot`（`FIELDNOTIFY` 字段）驱动。

理由（来源 WIKI §10.5）：HUD 值频繁变、来源多，手动绑定更灵活；存档界面纯表单，FieldNotify 双向绑定写起来快 3 倍。

**性能层（战斗表现）** —— 项目**不用**标准的 `UGameplayCueNotify_*` 资产，而是两个常驻子组件订阅 ASC 委托（`RegisterGameplayTagEvent` / `ActivatePassiveEffect`）。改 Debuff 表现时改 `DeBuff/DebuffNiagaraComponent`，不要去造 GameplayCue 资产。

---

## AI

- `AAuraAIController` 持 `BehaviorTreeComponent` + `BlackboardComponent`；`OnPossess` 时 `RunBehaviorTree(AAuraEnemy::BehaviorTree)`。
- `UBTService_FindNearestPlayer` — Tick 找最近玩家写入黑板 `TargetToFollow`。
- `UBTTask_Attack` — 调 `Execute_GetAttackMontages()` 挑蒙太奇，**通过 `UAbilitySystemBlueprintLibrary::SendGameplayEventToActor` 触发 Event 类型的 Ability**，AI 不直接调具体 C++ 函数。
- 黑板键：`TargetToFollow`（AActor*）、`MoveToLocation`（Vector）、`WhatClass`（ECharacterClass）。

---

## 存档 / 关卡切换

**`ISaveInterface`**（`Source/Aura/Public/Interaction/SaveInterface.h`）两个 `BlueprintNativeEvent`：`ShouldLoadTransform()`（Actor 决定要不要被还原）、`LoadActor()`。

**`ACheckpoint`** 继承 PlayerStart，含 `bReached`（`SaveGame` UPROPERTY）+ 蓝图中处理 Sphere Overlap 广播。`AMapEntrance : ACheckpoint` 额外含 `TSoftObjectPtr<UWorld> DestinationMap` + `DestinationPlayerStartTag`，触发 `TravelToMap`。

`AAuraGameModeBase::ChoosePlayerStart_Implementation` 读 SaveGame 里的 `PlayerStartTag`，从上次 Checkpoint 重生。

---

## 死亡流程（`AAuraCharacterBase::Die` → `MulticastHandleDeath`）

Reliable Multicast 在所有端执行：武器 `SetSimulatePhysics(true)` + Impulse、Mesh → Ragdoll、Collision 切 `Ragdoll`、`Dissolve()` 创建动态材质实例 → 蓝图 `StartDissolveTimeline` + `StartWeaponDissolveTimeline`。Player 启动 5s 定时器开存档界面；Enemy `SetLifeSpan(5.f)` + `SpawnLoot()`（蓝图查 `LootTiers` 概率生成 `AuraEffectActor`）。

**飘字走 `Client_ShowDamageNumber` 而非 `MulticastHandleDeath` 风格 Multicast** —— 飘字只该让造成这次伤害的那个玩家看见，服务器在 `PostGameplayEffectExecute` 里根据 `SourceController` 精准 Client RPC。

---

## 调试 / 修改代码时的心智模型

改属性层 → 改 `UAuraAttributeSet` 三个回调，注意 Clamp 与 `IncomingDamage` 中转路径。
改伤害计算 → 改 `ExecCalc_Damage` 的捕获属性表（首部 `FGameplayEffectAttributeCaptureDefinition`）。
改 Debuff 概率/DoT → 调 `DetermineDebuff()` 与 `UAuraAttributeSet::Debuff()`，Duration/Damage/Frequency 都通过 `SetByCaller` 从 Ability 传。
改技能状态机 → 入口 RPC 在 `UAuraAbilitySystemComponent`（`ServerSpendSpellPoint / ServerEquipAbility / UpdateAbilityStatuses`）。
改输入绑定 → 改 `DA_InputConfig` 资产，不动 C++。
改 UI → Widget 不直接读 ASC；只走 WidgetController，初值推送 + 订阅绑定两阶段不能反。
改战斗表现 VFX → 改 `DebuffNiagaraComponent` / `PassiveNiagaraComponent`，不要造 GameplayCue 资产。
改 AI 行为 → 走黑板键与 `SendGameplayEventToActor`，不要让 BT 直接调 C++。

详细章节、文件路径、代码示例以 [`WIKI.md`](WIKI.md) 为准（源码变动后请同步校对）。
