// Huanyu,miaomiaomiao,nekoneko


#include "MyAssetManager.h"

#include "MyGameplayTags.h"
#include "AbilitySystemGlobals.h"
//#include "AbilitySystem/AuraAbilitySystemComponent.h"

UMyAssetManager& UMyAssetManager::Get()
{
	/*
	 * 我们要先判断引擎，然后通过引擎获取到资源管理器的单例，
	 * 获取到的是对其的引用，我们返回时需要加*来返回它的实例
	 */
	check(GEngine);

	UMyAssetManager* MyAssetManager = Cast<UMyAssetManager>(GEngine->AssetManager);
	return *MyAssetManager;
}

void UMyAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//在此对FMyGameplayTags初始化
	FMyGameplayTags::InitializeNativeGameplayTags();

	// UE5.4版本可以不用调用，引擎自己调用了
	//如果使用TargetData，必须开启此项
	UAbilitySystemGlobals::Get().InitGlobalData();
}
