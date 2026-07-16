// Huanyu,miaomiaomiao,nekoneko


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	
}

void AAuraEffectActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//更新当前Actor的存在时间
	RunningTime += DeltaSeconds;
	ItemMovement(DeltaSeconds);
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	//设置初始位置
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// 如果触发角色类型为敌人，并且此拾取物设置无法被敌人拾取
	//if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	//使用ASC创建一个EffectContextHandle，这个句柄里面包含实例化GameplayEffect所需的数据
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	//设置此句柄的创建者
	EffectContextHandle.AddSourceObject(this);
	/**
	* 通过MakeOutgoingSpec()函数实例化GameplayEffect，
	* 返回一个实例化的句柄（句柄的意思是可以跟踪实例化对象的位置，即使它位置变动了也能跟踪的到）
	* MakeOutgoingSpec()函数需要传入三个值，
	* 第一个就是需要实例化的类，
	* 第二个是效果等级（它是有等级区分的），
	* 第三个值就是我们之前创建的实例化所需数据的句柄。
	*/
	//Effect的实例化后的句柄，可以通过此来寻找调用
	const FGameplayEffectSpecHandle EffectSpecHandle =
		TargetASC->MakeOutgoingSpec(GameplayEffectClass,ActorLevel, EffectContextHandle);
	//从句柄中获取到实例的地址，并被应用。
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	//从句柄中获取到定义的对象，并判断设置的
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	//在是无限时间效果和需要在结束时清除掉时，将效果句柄添加到map
	if(bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}
	else if (bDestroyOnEffectApplication)
	{
		// 如果设置了应用时删除，除了Infinite的都会自动删除
		Destroy();
	}
	// if (!bIsInfinite)
	// {
	// 	Destroy();
	// }
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// 如果触发角色类型为敌人，并且此拾取物设置无法被敌人拾取
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// 如果触发角色类型为敌人，并且此拾取物设置无法被敌人拾取
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if(InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if(!IsValid(TargetASC)) return;

		//创建存储需要移除的效果句柄存储Key，用于遍历完成后移除效果
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;

		//循环map内存的数据
		for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			//判断是否ASC相同
			if(TargetASC == HandlePair.Value)
			{
				//通过句柄将效果移除，注意，有可能有多层效果，不能将其它层的效果也移除掉，所以只移除一层
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				//添加到移除列表
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		//遍历完成后，在Map中将移除效果的KeyValue删除
		for(auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

void AAuraEffectActor::ItemMovement(float DeltaSeconds)
{
	//更新转向
	if(bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaSeconds * RotationRate, 0.f);
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}
	//更新位置
	if(bSinusoidalMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriod * 6.28318f);
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}
}



