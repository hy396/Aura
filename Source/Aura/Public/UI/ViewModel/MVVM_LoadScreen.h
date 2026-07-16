// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:

	
	void InitializeLoadSlots();

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;
	
	// 每个存档插槽使用的MVVM类
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	//创建新存档按下事件
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnterName);

	//开始新游戏按下事件
	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	//选择存档按下事件
	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	/**
	 * 删除存档
	 */
	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();
	
	//从存档读取数据，并设置每个插槽
	void LoadData();

	//void SetNumLoadSlots(int32 InNumLoadSlots);
	
	void SetWidgetName(const FString& InSlotName);
	FString GetWidgetName() const { return WidgetName; };
	
private:

	//用户控件的名称
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString WidgetName;
	
	//索引和对应MVVM实例的映射
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	//对象对MVVM实例的引用，防止垃圾回收机制对其进行回收
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	UPROPERTY()
	UMVVM_LoadSlot* SelectedSlot;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"));
	// int32 NumLoadSlots;
};

