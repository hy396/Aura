// Huanyu,miaomiaomiao,nekoneko


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_1->SlotIndex = 1;
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(2, LoadSlot_2);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlot_2->SlotIndex = 2;

	//SetNumLoadSlots(LoadSlots.Num());
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode))
	{
		GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Magenta, FString("请切换到单人模式"));
		return;
	}
	LoadSlots[Slot]->SetMapName(GameMode->DefaultMapName); //设置地图名称
	LoadSlots[Slot]->SetPlayerName(EnterName); //修改MVVM上存储的角色名称
	LoadSlots[Slot]->SlotStatus = Taken; //修改进入界面为加载界面
	LoadSlots[Slot]->SetPlayerLevel(1); //修改角色等级
	
	LoadSlots[Slot]->PlayerStartTag = GameMode->DefaultPlayerStartTag; // 设置玩家起始位置
	LoadSlots[Slot]->MapAssetName = GameMode->DefaultMap.ToSoftObjectPath().GetAssetName();
	
	
	GameMode->SaveSlotData(LoadSlots[Slot], Slot);//保存数据
	LoadSlots[Slot]->InitializeSlot(); //调用初始化

	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GameMode->GetGameInstance());
	GameInstance->LoadSlotName = LoadSlots[Slot]->GetLoadSlotName();
	GameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
	GameInstance->PlayerStartTag = GameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		// 删除存档
		AAuraGameModeBase::DeleteSlotData(SelectedSlot->GetLoadSlotName(), SelectedSlot->SlotIndex);
		// 修改用户控件显示
		SelectedSlot->SlotStatus = Vacant;//修改为创建存档
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GameMode->GetGameInstance());
	GameInstance->LoadSlotName = SelectedSlot->GetLoadSlotName();
	GameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;
	GameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	if (IsValid(SelectedSlot))
	{
		GameMode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::LoadData()
{
	//获取到加载存档界面的GameMode
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		//遍历映射，获取对应存档
		for(const TTuple<int32, UMVVM_LoadSlot*> Slot : LoadSlots)
		{
			const ULoadScreenSaveGame* SaveGame = GameMode->GetSaveSlotData(Slot.Value->GetLoadSlotName(), Slot.Key);

			//获取存档数据
			const FString PlayerName = SaveGame->PlayerName;
			const TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveGame->SaveSlotStatus;

			//设置存档视图模型数据
			Slot.Value->SetPlayerName(PlayerName);
			Slot.Value->SetMapName(SaveGame->MapName);
			Slot.Value->SetPlayerLevel(SaveGame->PlayerLevel);
			Slot.Value->SlotStatus = SaveSlotStatus;
			Slot.Value->PlayerStartTag = SaveGame->PlayerStartTag;

			//调用视图模型初始化
			Slot.Value->InitializeSlot();
		}
	}
}

void UMVVM_LoadScreen::SetWidgetName(const FString& InSlotName)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(WidgetName, InSlotName))
	{
		// UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent); //通过宏调用其它函数的广播
	}
}
