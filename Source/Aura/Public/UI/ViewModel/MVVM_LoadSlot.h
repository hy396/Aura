// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// 切换存档显示的用户控件的委托
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;

	void InitializeSlot();
	
	// 当前视图模型的索引，对应存档的索引
	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex;
	
	// 当前进入加载存档界面时，此存档应该显示的用户控件界面。
 	UPROPERTY()
 	TEnumAsByte<ESaveSlotStatus> SlotStatus;
	
	// 存储玩家关卡出生位置的标签
 	UPROPERTY()
 	FName PlayerStartTag;
	

 	UPROPERTY()
 	FString MapAssetName;
	/** Field Notifies */
	


	void SetPlayerName(FString InPlayerName);
	void SetMapName(FString InMapName);
	void SetPlayerLevel(int32 InLevel);
 	void SetLoadSlotName(FString InLoadSlotName);

	FString GetPlayerName() const { return PlayerName; }
	FString GetMapName() const { return MapName; }
	int32 GetPlayerLevel() const { return PlayerLevel; }
 	FString GetLoadSlotName() const { return LoadSlotName; }

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	int32 PlayerLevel;

	//UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString LoadSlotName;
};
