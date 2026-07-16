// Huanyu,miaomiaomiao,nekoneko


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlot, int32 SlotIndex) const
{
	// 检查是否有对应名称的存档
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIndex))
	{
		// 删除已保存的存档
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
	}

	// 创建新的存档
	USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(ULoadScreenSaveGame::StaticClass());
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGame);

	// 设置需要保存的数据
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotName = LoadSlot->GetLoadSlotName();
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->SlotIndex = SlotIndex;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName = LoadSlot->MapAssetName;
	LoadScreenSaveGame->PlayerLevel = LoadSlot->GetPlayerLevel();
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	// 保存存档
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject;
	// 检查是否有对应名称的存档
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// 读取存档
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}else
	{
		// 创建新的存档
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	// 将存档转换为 ULoadScreenSaveGame
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);

	return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlotData(const FString& SlotName, int32 SlotIndex)
{
	// 检查是否有对应名称的存档
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		// 删除已保存的存档
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	const UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	// 从游戏实例获取到存档的名称和索引
	const FString SlotName = GameInstance->LoadSlotName;
	const int32 SlotIndex = GameInstance->LoadSlotIndex;

	// 从游戏实例获取到存档的名称和索引
	return GetSaveSlotData(SlotName, SlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	// 修改下次复活的检测点
	GameInstance->PlayerStartTag = SaveObject->PlayerStartTag;

	// 从游戏实例获取到存档的名称和索引
	const FString SlotName = GameInstance->LoadSlotName;
	const int32 SlotIndex = GameInstance->LoadSlotIndex;
	
	// 保存存档
	UGameplayStatics::SaveGameToSlot(SaveObject, SlotName, SlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	// 获取关卡名称
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	// 获取到游戏实例
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	//获取存档
	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		if (DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
		}
		// 如果存档不包含对应关卡内容，将创建一个对应的数据结构体存储
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}

		// 获取对应的存档关卡数据结构体
		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty(); // 清空存储的内容

		// 使用迭代器遍历场景里的每一个Actor,将需要保存的Actor添加到对应的数据结构体中
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			//判断Actor是否存在，并判断Actor是否需要存储
			if(!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			// 创建存储结构体
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			// 创建一个 FMemoryWriter, 用于将数据写入 SavedActor.Bytes
			FMemoryWriter MemoryWriter(SavedActor.Bytes);

			// 创建一个序列化器, 将对象的成员以名称和值的形式保存到 FMemoryWriter
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true; // 设置序列化方式保存到存档的模式

			// 将Actor所需要保存的数据写入到 Archive, Archive 将把数据存储到 SavedActor.Bytes
			Actor->Serialize(Archive);

			SavedMap.SavedActors.AddUnique(SavedActor);
		}

		// 找到对应的名称结构体, 将数据存储到存档对象内
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if(MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}

		// 保存存档
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	// 获取关卡名称
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	// 获取到游戏实例
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	// 判断获取的存档是否存在
	if (UGameplayStatics::DoesSaveGameExist(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		// 获取到存档
		ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("加载对应存档失败"));
		}

		// 判断存档是否含有对应关卡数据
		if (SaveGame->HasMap(WorldName))
		{
			//获取到关卡内的所有存档数据
			TArray<FSavedActor> SavedActors = SaveGame->GetSavedMapWithMapName(WorldName).SavedActors;
			
			// 遍历场景内的所有Actors, 寻找存档内对应的数据并应用到场景
			for (FActorIterator It(World); It; ++It)
			{
				AActor* Actor = *It;

				if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

				// 遍历存档里对应关卡的所有Actor数据
				for (FSavedActor SavedActor : SavedActors)
				{
					// 查找到对应的Actor的存档数据
					if (SavedActor.ActorName == Actor->GetFName())
					{
						//判断当前Actor是否需要设置位置变换
						if(ISaveInterface::Execute_ShouldLoadTransform(Actor))
						{
							Actor->SetActorTransform(SavedActor.Transform);
						}

						// 反序列化，创建一个FMemoryReader实例用于从二进制数据中读取内容
						FMemoryReader MemoryReader(SavedActor.Bytes);

						//FObjectAndNameAsStringProxyArchive 代理类，用于序列化和反序列化对象的属性 true：表示允许使用字符串形式的对象和属性名称（便于调试和可读性）。
						FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
						Archive.ArIsSaveGame = true; // 指定反序列化是用于加载存档数据
						Actor->Serialize(Archive); // 执行反序列化,将二进制数据设置到Actor属性上

						// 修改Actor上的属性后，调用函数更新Actor的显示
						ISaveInterface::Execute_LoadActor(Actor);
					}
				}
			}
		}
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	//const FString SlotName = Slot->GetLoadSlotName();
	//const int32 SlotIndex = Slot->SlotIndex;

	// 打开地图
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (const TTuple<FString, TSoftObjectPtr<UWorld>>& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	const UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	//获取关卡里的所有PlayerStart实例
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		// 获取到第一个实例对象
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				// 判断PlayerStart的Tag设置是否为指定的Tag
				if (PlayerStart->PlayerStartTag == GameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}

	return nullptr;
}

void AAuraGameModeBase::PlayerDied(const ACharacter* DeadCharacter) const
{
	// 获取存档数据
	const ULoadScreenSaveGame* SaveGame = RetrieveInGameSaveData();
	if (!IsValid(SaveGame)) return;

	// 通过地图命名打开地图
	UGameplayStatics::OpenLevelBySoftObjectPtr(DeadCharacter, Maps.FindChecked(SaveGame->MapName));
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
