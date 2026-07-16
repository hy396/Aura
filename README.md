<br>

<div align="center">

# Aura

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.4-blue?style=flat)](#)
[![C++](https://img.shields.io/badge/C%2B%2B-20-red?style=flat)](#)
[![Framework](https://img.shields.io/badge/Framework-GAS-orange?style=flat)](#)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey?style=flat)](#)
[![Mode](https://img.shields.io/badge/Mode-Single%2BLocal%20Co--op-yellowgreen?style=flat)](#)
[![Docs](https://img.shields.io/badge/Docs-WIKI.md-informational?style=flat)](#)

</div>

<br>

## 项目简介

Aura 是一款基于 Unreal Engine 5 开发的俯视角 RPG 游戏，采用了 GameplayAbilitySystem (GAS) 框架构建了完整的角色能力系统、技能战斗系统和 UI 系统。游戏融合了角色扮演、技能成长和战斗策略等多种元素，提供了丰富的游戏体验。

## 技术架构

### 核心框架
- **Unreal Engine 5**: 作为游戏开发引擎
- **GameplayAbilitySystem (GAS)**: 用于实现角色能力、技能系统和属性计算
- **Enhanced Input System**: 用于处理玩家输入
- **ModelViewViewModel (MVVM)**: 用于构建数据驱动的 UI 系统

### 主要系统

#### 1. 能力系统 (Ability System)
- 基于 GAS 框架实现完整的技能系统
- 支持主动技能和被动技能
- 技能冷却、装备、升级等管理机制
- 输入绑定与状态管理

#### 2. 属性系统 (Attribute System)
- 主要属性：力量、智力、韧性、耐力
- 次要属性：血量、法力、护甲、暴击等
- 抗性系统：火、雷、奥术、物理四种元素抗性
- 网络同步与实时 UI 刷新

#### 3. 战斗系统 (Combat System)
- 技能伤害计算模块
- DeBuff 系统（燃烧、眩晕、流血等）
- 格挡、暴击等战斗机制
- 受击反应与死亡处理

#### 4. UI 系统
- 采用 MVC 架构设计
- 数据驱动的界面更新机制
- Overlay 界面：显示角色血量、法力等核心信息
- 技能菜单：技能升级与装备管理
- 数据绑定与动态委托监听

#### 5. 角色系统
- 多职业设计（法师、战士、游侠）
- 角色属性成长与技能树
- AI 行为与战斗决策系统
- 动画蒙太奇驱动的战斗逻辑

## 项目结构

```
├── Config/                 # 配置文件
├── Data/                   # 数据文件与简历
├── Plugins/                # 第三方插件
│   ├── VRM4U               # VRM 模型支持
│   ├── KawaiiPhysics        # 物理模拟插件
│   └── RiderLink           # Rider IDE 集成
├── Source/                 # 源代码
│   ├── Aura/              # 主模块
│   │   ├── AbilitySystem/ # 能力系统
│   │   ├── AI/            # AI 相关
│   │   ├── Character/     # 角色系统
│   │   ├── Game/          # 游戏模式与逻辑
│   │   ├── Input/         # 输入系统
│   │   ├── Interaction/   # 交互系统
│   │   ├── Player/        # 玩家控制器与状态
│   │   └── UI/            # UI 系统
└── Content/               # 游戏资源
```

## 核心特性

### GameplayAbilitySystem 集成
- 完整的技能生命周期管理
- 技能配置与规格化系统
- 技能执行与冷却系统
- 主动与被动技能管理

### 网络同步
- 属性系统网络同步
- 技能状态同步机制
- RPC 调用与客户端预测
- 带宽优化策略

### UI 系统
- MVVM 架构实现
- 数据绑定与命令处理
- WidgetController 系统
- UI 性能优化

### 第三方插件集成
- VRM4U: 支持 VRM 角色模型导入与使用
- KawaiiPhysics: 提供物理模拟效果
- RiderLink: Rider IDE 开发支持

## 开发环境

- **引擎版本**: Unreal Engine 5.4
- **编程语言**: C++
- **开发工具**: Visual Studio/Rider
- **版本控制**: Git

## 主要技术亮点

1. **基于 GAS 的角色属性系统**：设计包含力量、智力、韧性、耐力等核心属性与元素抗性的动态计算框架
2. **技能伤害计算模块**：集成元素抗性机制和 DeBuff 系统，通过 GameplayEffect 实现伤害计算和状态效果应用
3. **MVC 架构的 UI 系统**：通过动态委托监听各属性变化并同步到 UI 界面，实现数据驱动的界面更新机制
4. **AI 与战斗系统优化**：融合行为树与环境查询系统，实现智能敌人寻路和战斗决策
5. **性能优化**：通过预编译宏和属性缓存机制减少反射调用，使用 GameplayTag 系统优化技能和状态管理

## 项目职责

- 主导开发基于 GAS 框架的角色属性系统
- 开发技能伤害计算模块，集成元素抗性机制和 DeBuff 系统
- 设计并实现 MVC 架构的 UI 系统
- 优化 AI 与战斗系统
- 性能优化与代码重构

## 文档

更详细的逐系统架构说明请参阅 [WIKI.md](WIKI.md)。

---

*幻雨ネコ*
