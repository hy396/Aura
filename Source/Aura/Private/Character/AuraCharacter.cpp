// Huanyu,miaomiaomiao,nekoneko


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AAuraCharacter::AAuraCharacter()
{
	//bReplicates = true;//服务器负责计算并更新Actor的状态，然后通过网络将这些更新复制到所有连接的客户端上。
	//设置相机
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent()); //设置附加组件
	LevelUpNiagaraComponent->bAutoActivate = false; //设置不自动激活

	DunSphere = CreateDefaultSubobject<USphereComponent>("DunSphere");
	DunSphere->SetupAttachment(GetRootComponent());
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
	
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//初始化ASC的OwnerActor和AvatarActor
	InitAbilityActorInfo();
	LoadProgress();
	//初始化角色技能
	//AddCharacterAbilities();

	if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->LoadWorldState(GetWorld());
	}
}

void AAuraCharacter::LoadProgress() const
{
	if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		//获取存档
		ULoadScreenSaveGame* SaveGameData = GameMode->RetrieveInGameSaveData();
		if(SaveGameData == nullptr)
		{
			// 没有存档也初始,测试我也要玩,初始内容跟第一次存档一样
			InitializeDefaultAttributes();
			AddCharacterAbilities();
			return;
		}
		// 判断是否为第一次加载存档，如果第一次，属性没有相关内容
		if (SaveGameData->bFirstTimeLoadIn)
		{
			//如果第一次加载存档，使用默认GE初始化主要属性
			InitializeDefaultAttributes();

			//初始化角色技能
			AddCharacterAbilities();
		}
		else
		{
			if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
			{
				AuraPlayerState->SetLevel(SaveGameData->PlayerLevel, false);
				AuraPlayerState->SetXP(SaveGameData->XP);
				AuraPlayerState->SetAttributePoints(SaveGameData->AttributePoints);
				AuraPlayerState->AddToSpellPoints(SaveGameData->SpellPoints);
			}
			
			//如果不是第一次，将通过函数库函数通过存档数据初始化角色属性
			UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveGameData);
			
			//初始化角色技能
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				AuraASC->AddCharacterAbilitiesFromSaveData(SaveGameData);
			}

			// 从存档中读取关卡状态
			GameMode->LoadWorldState(GetWorld());
		}
	}

}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//初始化ASC的OwnerActor和AvatarActor
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	PlayerStateBase->AddToXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}
void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		//const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		//const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		//const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation(); //获取相机位置和粒子特效的朝向
		//LevelUpNiagaraComponent->SetWorldRotation(FRotator()); //设置粒子的转向
		LevelUpNiagaraComponent->Activate(true); //激活特效
	}
}
int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	return PlayerStateBase->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	if (Level >= PlayerStateBase->LevelUpInfo->LevelUpInformation.Num()) return 0;
	return PlayerStateBase->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	if (Level >= PlayerStateBase->LevelUpInfo->LevelUpInformation.Num()) return 0;
	return PlayerStateBase->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	return PlayerStateBase->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	return PlayerStateBase->GetSpellPoints();
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	PlayerStateBase->AddToLevel(InPlayerLevel);

	if (UAuraAbilitySystemComponent* AuraAsc = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraAsc->UpdateAbilityStatuses(PlayerStateBase->GetPlayerLevel());
	}
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	PlayerStateBase->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	PlayerStateBase->AddToSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	return AuraPlayerState->GetPlayerLevel();
}

USphereComponent* AAuraCharacter::GetDunSphereComponent_Implementation()
{
	return DunSphere;
}

void AAuraCharacter::SetDunSphereComponent_Implementation(bool bInCollision)
{
	if (bInCollision)
	{
		DunSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}else
	{
		DunSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
	}
}

void AAuraCharacter::Die_Implementation(const FVector& DeathImpulse)
{
	Super::Die_Implementation(DeathImpulse);

	// 创建一个委托用于绑定委托回调
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			GameMode->PlayerDied(this);
		}
	});

	// 通过定时器触发对应的委托回调
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);

	//防止相机在玩家角色死亡后跟随移动，将相机固定在世界坐标位置
	TopDownCameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHold);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunDeBuffComponent->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDeBuffComponent->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDeBuffComponent->Activate();
	}
	else
	{
		BurnDeBuffComponent->Deactivate();
	}
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);

		// 关闭鼠标显示
		AuraPlayerController->SetShowMouseCursor(false);
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();

		// 显示鼠标
		AuraPlayerController->SetShowMouseCursor(true);
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		//获取存档
		ULoadScreenSaveGame* SaveGameData = GameMode->RetrieveInGameSaveData();
		if(SaveGameData == nullptr) return;

		//修改存档数据
		SaveGameData->PlayerStartTag = CheckpointTag;

		//修改玩家相关
		if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
		{
			SaveGameData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveGameData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveGameData->XP = AuraPlayerState->GetXP();
			SaveGameData->AttributePoints = AuraPlayerState->GetAttributePoints();
			SaveGameData->SpellPoints = AuraPlayerState->GetSpellPoints();
		}
		//修改主要属性
		SaveGameData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

		SaveGameData->bFirstTimeLoadIn = false;//保存完成将第一次加载属性设置为false
		// 如果当前对象没有网络权限（非服务器），则不执行保存操作
		if (!HasAuthority()) return;

		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		SaveGameData->SavedAbilities.Empty(); //清空保存的技能

		// 使用ASC里创建的ForEach函数循环获取角色技能，并生成技能结构体保存
		FForEachAbility SaveAbilityDelegate;
		// 绑定一个 lambda 函数到 SaveAbilityDelegate，用于遍历当前角色的所有技能（FGameplayAbilitySpec）
		// 并将每个技能的信息提取后保存到存档数据（SaveGameData）中。
		SaveAbilityDelegate.BindLambda([this, AuraASC, SaveGameData](const FGameplayAbilitySpec& AbilitySpec)
		{
			// 从技能规范中获取技能的 GameplayTag 标识符
			const FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);

			// 获取与该技能相关的 UAbilityInfo 对象
			UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);

			// 通过 AbilityInfo 查找对应技能标签的详细信息 FAuraAbilityInfo
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			// 创建 FSavedAbility 结构体实例，用于保存当前技能的相关数据
			FSavedAbility SavedAbility{
				.GameplayAbility = Info.Ability,									// 设置技能类
				.AbilityLevel = AbilitySpec.Level,									// 设置技能等级
				.AbilityTag = AbilityTag,											// 设置技能的 GameplayTag
				.AbilityStatus = AuraASC->GetStatusTagFromAbilityTag(AbilityTag),	// 获取并设置技能的状态标签
				.AbilityInputTag = AuraASC->GetInputTagFromAbilityTag(AbilityTag),	// 获取并设置技能的输入标签
				.AbilityType = Info.AbilityType										// 设置技能的类型（例如主动技能或被动技能）
			};
			// 将当前技能的保存数据添加到存档对象中的技能列表里，确保唯一性
			SaveGameData->SavedAbilities.AddUnique(SavedAbility);
		});
		
		//调用ForEach技能来执行存储到存档
		AuraASC->ForEachAbility(SaveAbilityDelegate);

		//保存存档
		GameMode->SaveInGameProgressData(SaveGameData);
	}
}

TSubclassOf<UGameplayEffect> AAuraCharacter::GetSecondaryAttributes_Implementation()
{
	return DefaultSecondaryAttributes;
}

TSubclassOf<UGameplayEffect> AAuraCharacter::GetVitalAttributes_Implementation()
{
	return DefaultVitalAttributes;
}

void AAuraCharacter::InitAbilityActorInfo()
{
	// 为能力角色信息初始化玩家状态和能力系统组件
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	
	//从playerState获取ASC和AS
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	//初始化ASC
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	//触发Actor的技能信息设置回调
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	//调用ASC广播
	OnAscRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FMyGameplayTags::Get().DeBuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);

	//获取PC
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	//改成第一次读档还有有存档情况下的初始化
	//通过GE初始角色主要属性
	//InitializeDefaultAttributes();
}
