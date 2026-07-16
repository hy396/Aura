// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "OverlayWidgetController.generated.h"


USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;

};

class UAuraUserWidget;
// 声明一个动态多播委托，该委托有一个参数，参数类型为float，参数名为NewHealth


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedSignature, int32, NewLevel, bool, bLevelUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnXPPercentChangedSignature, int32, XPSinceLastLevel, int32, XPRequiredForCurrentLevel, float, NewValue);

/**
 * 屏幕覆盖用户控件控制器层基类，继承与用户控件控制器
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;
	
	// 经验条百分比变动回调
	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnXPPercentChangedSignature OnXPPercentChangedDelegate;
	
	// 等级变动回调
	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnLevelChangedSignature OnPlayerLevelChangedDelegate;

protected:

	// 信息表格数据
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	// 经验变动后的回调
	void OnXPChanged(int32 NewXP);
	
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const;
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
