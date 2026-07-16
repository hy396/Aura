// Huanyu,miaomiaomiao,nekoneko


#include "UI/HUD/LoadScreenHUD.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	// 实列化MVVM
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->SetWidgetName("WidgetName"); //测试代码
	LoadScreenViewModel->InitializeLoadSlots(); //初始化插槽使用的MVVM
	
	// 创建用户控件并添加到视口
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();

	// 获取玩家控制器
	APlayerController *PC = GetOwningPlayerController();
	// 设置UI输入模式
	FInputModeUIOnly InputMode;
	
	// 将输入焦点设置到加载屏幕用户控件上，使UI能够接收输入事件
	InputMode.SetWidgetToFocus(LoadScreenWidget->TakeWidget());

	// 设置鼠标不锁定在游戏视口，允许用户自由移动鼠标（通常用于UI交互场景）
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// 设置输入模式
	PC->SetInputMode(InputMode);
	
	// 显示鼠标光标，允许用户在游戏视口上移动鼠标
	PC->SetShowMouseCursor(true);

	//创建完成用户控件后，调用用户控件函数
	LoadScreenWidget->BlueprintInitializeWidget();

	//加载存档
	LoadScreenViewModel->LoadData();
}
