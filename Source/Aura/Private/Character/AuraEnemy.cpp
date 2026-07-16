// Huanyu,miaomiaomiao,nekoneko


#include "Character/AuraEnemy.h"

#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UI/Widget/AuraUserWidget.h"


AAuraEnemy::AAuraEnemy()
{
	bReplicates = true;//服务器负责计算并更新Actor的状态，然后通过网络将这些更新复制到所有连接的客户端上。

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	// 设置组件用于在网络上复制
	AbilitySystemComponent->SetIsReplicated(true);
	/**
	*	Minimal：仅复制最小的游戏效果信息。注意，此模式不适用于拥有的AbilitySystemComponents（应使用Mixed模式）。
	*	Mixed：对模拟代理只复制最小的游戏效果信息，但对所有者和自主代理复制完整信息。
	*	Full：向所有对象复制完整的游戏效果信息。
	*/
	//AI
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent()); //将血条附件到根节点上

	// 让入机的旋转变得丝滑的操作
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	//BaseWalkSpeed = 250.f;

}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//当前设置只在服务器端运行
	if (!HasAuthority()) return;
	//AIController是在服务器端执行的，所以需要在PossessedBy函数回调中获取服务器返回
	AuraAIController = Cast<AAuraAIController>(NewController);
	//初始化行为树上设置的黑板
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	//运行行为树
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);

}

void AAuraEnemy::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//InitAbilityActorInfo();
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAuraEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	//GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	//Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	// 敌人类里，将不修改移动位置
}

int32 AAuraEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

void AAuraEnemy::Die_Implementation(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	//设置死亡，停止AI行为树
	if(AuraAIController) AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);

	SpawnLoot(); // 掉落物品
	
	Super::Die_Implementation(DeathImpulse);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	// 设置角色的初始移动速度
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	// 初始化角色的ASC
	InitAbilityActorInfo();
	//初始化角色的技能
	if (HasAuthority())
	{
		// 服务器下进行初始化
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this,AbilitySystemComponent,CharacterClass);
	}
	/*
	 * 我们要将敌人的基类作为控制器设置给用户控件，
	 * 可以在用户控件绑定对应监听，我们在初始化完成后，
	 * 开始执行事件时，从用户控件组件中获取到用户控件，
	 * 转换为我们创建的用户控件基类的实力，
	 * 调用方法设置。这个方法会在设置后，触发设置回调
	 */
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}
	if (const UAuraAttributeSet* AuraAs = Cast<UAuraAttributeSet>(AttributeSet))
	{
		// 监听血量变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAs->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAs->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
				//UE_LOG(LogTemp, Warning, TEXT("%s 的最大生命值为 %f"), *this->GetName(), Cast<UAuraAttributeSet>(AttributeSet)->GetMaxHealth());
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FMyGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AAuraEnemy::HitReactTagChanged
		);
		// 初始化血量
		OnHealthChanged.Broadcast(AuraAs->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAs->GetMaxHealth());
		//UE_LOG(LogTemp, Warning, TEXT("%s 的生命值为 %f"), *this->GetName(), AuraAs->GetHealth());
		//UE_LOG(LogTemp, Warning, TEXT("%s 的生命值为 %f"), *this->GetName(), Cast<UAuraAttributeSet>(AttributeSet)->GetHealth());
	}
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		//设置黑板键的值
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
	
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AbilitySystemComponent->RegisterGameplayTagEvent(FMyGameplayTags::Get().DeBuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::StunTagChanged);

	// 服务器下进行初始化
	if (HasAuthority())
	{
		// 通过GE初始角色的属性
		InitializeDefaultAttributes();
	}
	//调用ASC广播
	OnAscRegistered.Broadcast(AbilitySystemComponent);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	//Super::InitializeDefaultAttributes();
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);

}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	// 设置黑板键的值
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
	}
}
