// Huanyu,miaomiaomiao,nekoneko


#include "UI/ViewModel/MVVM_LoadSlot.h"


void UMVVM_LoadSlot::InitializeSlot()
{
	// 从枚举获取到对应的索引
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	// 广播切换
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_LoadSlot::SetPlayerLevel(int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InLevel);
}

void UMVVM_LoadSlot::SetLoadSlotName(FString InLoadSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InLoadSlotName);
}

