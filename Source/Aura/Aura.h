// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

//ECollisionChannel::ECC_GameTraceChannel1代表着我们添加的碰撞类型的第一个类型
#define ECC_PROJECTILE ECollisionChannel::ECC_GameTraceChannel1 //对投掷物响应的通道
#define ECC_Target ECollisionChannel::ECC_GameTraceChannel2 //技能对攻击目标拾取的通道，只包含场景中的角色
#define ECC_ExcludePlayer ECollisionChannel::ECC_GameTraceChannel3 //技能范围选择时的通道，忽略场景中可动的角色
