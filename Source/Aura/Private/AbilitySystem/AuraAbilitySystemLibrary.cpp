// Huanyu,miaomiaomiao,nekoneko


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,
	FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
	//获取到playerController， 需要传入一个世界空间上下文的对象，用于得到对应世界中的PC列表，0为本地使用的PC
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		//从PC获取到HUD，从HUD获得对应的Controller
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());
		if (OutAuraHUD)
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
			UAttributeSet* As = PS->GetAttributeSet();

			OutWCParams.AttributeSet = As;
			OutWCParams.AbilitySystemComponent = Asc;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
                                                            ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();
	// 从实例获取到关卡角色的配置
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	// 获取到默认的基础角色数据
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	// 应用基础属性
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());
	
	// 应用次级属性
	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());
	
	// 填充血量和蓝量
	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject,
	UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	AActor* AvatarActor = ASC->GetAvatarActor();

	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();

	// 从实例获取到关卡角色的配置
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if(CharacterClassInfo == nullptr) return;

	//*********************************初始化主要属性*********************************

	// 创造GE的上下文句柄
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	// 设置生成此效果的源对象
	EffectContextHandle.AddSourceObject(AvatarActor);

	// 根据句柄和类创建GE实例，并通过句柄找到GE实例
	const FGameplayEffectSpecHandle PrimaryContextHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryAttributes_SetByCaller, 1.0f, EffectContextHandle);
	
	//通过标签设置GE使用的配置
	/*
	* SpecHandle：要修改的 GameplayEffect 规格句柄。
	* Tag：你要设置的 Gameplay Tag。
	* Magnitude：你希望这个 Tag 对应的数值是多少。
	 */
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryContextHandle, GameplayTags.Attributes_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryContextHandle, GameplayTags.Attributes_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryContextHandle, GameplayTags.Attributes_Primary_Resilience, SaveGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryContextHandle, GameplayTags.Attributes_Primary_Vigor, SaveGame->Vigor);

	//应用GE
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryContextHandle.Data.Get());

	if (AvatarActor->Implements<UPlayerInterface>())
	{
		//*********************************设置次级属性*********************************

		FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
		SecondaryContextHandle.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(IPlayerInterface::Execute_GetSecondaryAttributes(AvatarActor), 1.0f, EffectContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

		//*********************************填充血量和蓝量*********************************

		// 创建句柄
		FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
		// 设置源对象
		VitalContextHandle.AddSourceObject(AvatarActor);
		// 根据句柄和类创建GE实例，并通过句柄找到GE实例
		const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(IPlayerInterface::Execute_GetVitalAttributes(AvatarActor), 1.0f, VitalContextHandle);
		// 应用GE
		ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
	}
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
                                                     ECharacterClass CharacterClass)
{
	// 从实例获取到关卡角色的配置
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	// 遍历角色拥有的技能数组
	for (const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1); // 默认等级1
		ASC->GiveAbility(AbilitySpec); //只应用不激活
	}
	// 赋予职业专属技能
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
	{
		for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
		{
			// 通过战斗接口获取当前角色等级
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec); // 根据角色等级初始化技能
		}
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 CharacterLevel)
{
	// 从实例获取到关卡角色的配置
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	// 获取到默认的基础角色数据
	const FCharacterClassDefaultInfo& ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	// 通过ClassDefaultInfo中的数据曲线传入等级获取对应等级的XP值
	const float XPReward = ClassDefaultInfo.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

ULootTiers* UAuraAbilitySystemLibrary::GetLootTiers(const UObject* WorldContextObject)
{
	//获取到当前关卡的GameMode实例
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr) return nullptr;

	//返回敌人战利品配置，需要设置到GameMode上
	return  GameMode->LootTiers;
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	//获取到当前关卡的GameMode实例
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;
	//返回关卡的角色的配置
	return GameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	// 获取到当前关卡的GameMode实例
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;

	//返回关卡的角色配置
	return GameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext *AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDeBuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsSuccessfulDeBuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDeBuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeBuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDeBuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeBuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDeBuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeBuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		// 如果当前存在设置了伤害类型
		if (AuraEffectContext->GetDamageType().IsValid())
		{
			//取消指针
			return *AuraEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDeBuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInSuccessfulDeBuff)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDeBuff(bInSuccessfulDeBuff);
	}
}

void UAuraAbilitySystemLibrary::SetDeBuff(FGameplayEffectContextHandle& EffectContextHandle, FGameplayTag& InDamageType,
	const float InDamage, const float InDuration, const float InFrequency)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		// 通过标签创建一个共享指针
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraEffectContext->SetDamageType(DamageType);
		AuraEffectContext->SetDeBuffDamage(InDamage);
		AuraEffectContext->SetDeBuffDuration(InDuration);
		AuraEffectContext->SetDeBuffFrequency(InFrequency);
	}
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	// 获取全局游戏标签管理器（用于标记伤害类型等）
	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
    
	// 获取施法者的AvatarActor（通常是角色Actor）
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	// 创建GameplayEffect的上下文句柄
	// 该句柄保存了技能释放时的环境信息（如位置、方向等）
	FGameplayEffectContextHandle EffectContextHandle = 
		DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();

	// 将施法者Actor添加到上下文源对象中（用于追踪效果来源）
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	// 设置力的相关参数
	SetForce(EffectContextHandle,
		DamageEffectParams.DeathImpulse,
		DamageEffectParams.KnockbackForce,
		DamageEffectParams.bIsRadialDamage,
		DamageEffectParams.RadialDamageInnerRadius,
		DamageEffectParams.RadialDamageOuterRadius,
		DamageEffectParams.RadialDamageOrigin);
	
	// 创建GameplayEffect的规格实例（Spec）,根据传入的GameplayEffect类和等级生成具体效果实例
	const FGameplayEffectSpecHandle SpecHandle = 
		DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
			DamageEffectParams.DamageGameplayEffectClass, // 要应用的伤害GE类
			DamageEffectParams.AbilityLevel,              // 技能等级
			EffectContextHandle                        // 上下文环境
		);
	
	// 设置伤害相关数值参数
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Chance, DamageEffectParams.DeBuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Damage, DamageEffectParams.DeBuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Duration, DamageEffectParams.DeBuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Frequency, DamageEffectParams.DeBuffFrequency);
	
	// 将生成的GameplayEffect应用到目标角色的AbilitySystemComponent
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return EffectContextHandle;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis,
	float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;
	// 获取到最左侧的角度
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		// 技能分的段数
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		// 如果只需要一个，则将朝向放入即可
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis,
	float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis); //获取到最左侧的角度
	
	if(NumVectors > 1)
	{
		const float DeltaSpread = Spread / NumVectors; //技能分的段数

		for(int32 i=0; i<NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i + 0.5f), Axis); //获取当前分段的角度
			Vectors.Add(Direction);
		}
	}
	else
	{
		//如果只需要一个，则将朝向放入即可
		Vectors.Add(Forward);
	}

	return Vectors;
}

// float UAuraAbilitySystemLibrary::ApplyRadialDamageWithFalloff(AActor* TargetActor, float BaseDamage,
// 	float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff,
// 	AActor* DamageCauser, AController* InstigatedByController, ECollisionChannel DamagePreventionChannel)
// {
// 	// 判断目标角色是否死亡
// 	bool bIsDead = true;
// 	if (TargetActor->Implements<UCombatInterface>())
// 	{
// 		bIsDead = ICombatInterface::Execute_IsDead(TargetActor);
// 	}
// 	if (bIsDead)
// 	{
// 		return 0.f; // 如果目标角色已经死亡，则不进行伤害计算
// 	}
//
// 	// 获取目标角色所有组件
// 	TArray<UActorComponent*> Components;
// 	TargetActor->GetComponents(Components);
//
// 	// 判断攻击是否能够查看目标
// 	bool bIsDamageable = false;
// 	// 存储目标受到碰撞查询到的碰撞结果
// 	TArray<FHitResult> HitList;
//
// 	for (UActorComponent* Component : Components)
// 	{
// 		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component);
// 		if (PrimitiveComponent && PrimitiveComponent->IsCollisionEnabled())
// 		{
// 			FHitResult Hit;
// 			bIsDamageable = ComponentIsDamageableFrom(
// 				PrimitiveComponent, Origin, DamageCauser, {}, DamagePreventionChannel, Hit
// 			);
// 		}
// 	}
//
// 	
// }

void UAuraAbilitySystemLibrary::SetForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpulse,
                                         const FVector& InForce, bool bInIsRadialDamage, float InnerRadius, float InOuterRadius, const FVector& InOrigin)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		// 设置死亡冲击力
		AuraEffectContext->SetDeathImpulse(InImpulse);
		// 设置击退力
		AuraEffectContext->SetKnockbackForce(InForce);

		
		// 设置是否为范围伤害
		AuraEffectContext->SetIsRadialDamage(bInIsRadialDamage);
		// 设置范围伤害的内半径
		AuraEffectContext->SetRadialDamageInnerRadius(InnerRadius);
		// 设置范围伤害的外半径
		AuraEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
		// 设置范围伤害的起始位置
		AuraEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams, bool bIsRadial,
	float InnerRadius, float InOuterRadius, FVector InOrigin)
{
	// 设置是否为范围伤害
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	// 设置范围伤害的内半径
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	// 设置范围伤害的外半径
	DamageEffectParams.RadialDamageOuterRadius = InOuterRadius;
	// 设置范围伤害的起始位置
	DamageEffectParams.RadialDamageOrigin = InOrigin;
}

void UAuraAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetTargetEffectParamsASC(FDamageEffectParams& DamageEffectParams,
	UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                           TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                           const FVector& SphereOrigin)
{
	// 步骤1：设置碰撞检测参数，忽略指定角色（如自己）
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore); // 添加忽略的Actor,比如要排除施法者自己

	// 步骤2：创建存储检索到的与碰撞体产生碰撞的Actor
	TArray<FOverlapResult> Overlaps; 

	// 步骤3：获取当前游戏场景（世界）, 如果获取失败，将打印并返回Null
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		// 步骤4：执行球形范围检测（核心碰撞检测逻辑）// 获取到所有与此球体碰撞的动态物体
		World->OverlapMultiByObjectType(
			Overlaps,                // [输出] 存储检测结果
			SphereOrigin,            // 圆心位置（比如技能释放点）
			FQuat::Identity,         // 不旋转
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius), // 检测范围形状（球形）
			SphereParams             // 包含忽略列表的参数
		);
		// 步骤5：筛选有效结果
		for(FOverlapResult& Overlap : Overlaps)
		{
			//判断当前Actor是否包含战斗接口   Overlap.GetActor() 从碰撞检测结果中获取到碰撞的Actor
			const bool ImplementsCombatInterface =  Overlap.GetActor()->Implements<UCombatInterface>();
			//判断当前Actor是否存活，如果不包含战斗接口，将不会判断存活（放置的火堆也属于动态Actor，这样保证不会报错）
			if(ImplementsCombatInterface && !ICombatInterface::Execute_IsDead(Overlap.GetActor())) 
			{
				OutOverlappingActors.AddUnique(Overlap.GetActor()); //将Actor添加到返回数组，AddUnique 只有在此Actor未被添加时，才可以添加到数组,避免重复添加
			}
		}
	}
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	// 如果数量过于少，直接返回原数组
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}
	// 没有引用就是复制，复制一份用于遍历
	TArray<AActor*> ActorsToCheck = Actors;
	// 当前已经遍历出最近距离的个数
	int32 NumTargetFound = 0;

	// 循环遍历，直到获取足够数量的目标时停止
	while (NumTargetFound < MaxTargets)
	{
		// 如果没有可遍历内容，跳出循环
		if (ActorsToCheck.Num() == 0) break;
		// 记录中心于目标的位置，如果有更小的将被替换，默认是最大
		double ClosestDistance = TNumericLimits<double>::Max();
		// 缓存当前最近距离的目标
		AActor* ClosestActor = nullptr;
		for (AActor* PotentialTarget: ActorsToCheck)
		{
			// 获取目标和中心的距离
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();

			// 对比当前计算的位置是否小于缓存的位置
			if (Distance < ClosestDistance)
			{
				// 如果是更小的，更新缓存
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}

		// 从遍历数组中删除缓存的对象
		ActorsToCheck.Remove(ClosestActor);
		// 添加到返回数组中
		OutClosestTargets.Add(ClosestActor);
		// 递增数量
		++NumTargetFound;
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	if(FirstActor->ActorHasTag("Player"))
	{
		return !SecondActor->ActorHasTag("Player");
	}
	if(FirstActor->ActorHasTag("Enemy"))
	{
		return !SecondActor->ActorHasTag("Enemy");
	}

	return false;
}


