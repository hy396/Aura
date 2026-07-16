// Huanyu,miaomiaomiao,nekoneko


#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();//绑定监听数值变化
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();//绑定监听数值变化
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);
		SpellMenuWidgetController->BindCallbacksToDependencies();//绑定监听数值变化
	}
	return SpellMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass 未设置，请在BP_HUD上面设置 "));
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass 未设置，请在BP_HUD上面设置"));

	//创建Overlay用户控件
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	
	//创建参数结构体
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	//获取控制器层
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	//初始化所有控制器
	GetAttributeMenuWidgetController(WidgetControllerParams);
	GetSpellMenuWidgetController(WidgetControllerParams);
	
	//设置用户控件的控制器层
	OverlayWidget->SetWidgetController(WidgetController);

	//初始化广播的值，在所有的UI绑定后，调用一次，实现数值在UI上的初始化
	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();//添加到视口
}

