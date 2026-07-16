// Huanyu,miaomiaomiao,nekoneko


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MyGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	// 设置 bReplicates 为 true，确保 PlayerController 在网络中同步
	bReplicates = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");

	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	// 鼠标位置追踪是否悬停在敌人身上
	CursorTrace();
	// 自动寻路
	AutoRun();
	// 更新魔法花圈的位置
	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if(!IsValid(MagicCircle)) MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);

	//设置贴花材质
	if(DecalMaterial != nullptr)
	{
		MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
	}
}

void AAuraPlayerController::HideMagicCircle() const
{
	if(IsValid(MagicCircle)) MagicCircle->Destroy();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter,
                                                            bool bBlockedHit, bool bCriticalHit, bool bIsExp)
{
	// 确保传入的目标没被销毁并且设置了组件类
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); //动态创建的组件需要调用注册
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); //先附加到角色身上，使用角色位置
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); //然后从角色身上分离，保证在一个位置播放完成动画
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit, bIsExp); //设置显示的伤害数字
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return ;
	if (APawn* ControlledPawn = GetPawn())
	{
		//找到距离样条最近的位置
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// 获取这个位置在样条上的方向
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		// 让角色通过此方向去移动
		ControlledPawn->AddMovementInput(Direction);
		/*
		 * 我们还需要在角色移动到终点时，设置它停止寻路，
		 * 所以，我们还需要查询当前角色所在位置和终点位置的距离，
		 * 如果小于我们设置的AutoRunAcceptanceRadius距离内，
		 * 我们将停止自动寻路。
		 */
		if(const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
			DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation() const
{
	if(IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	//check(Subsystem);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	//设置鼠标光标可见
	bShowMouseCursor = true;
	//设置鼠标光标的样式为箭头光标
	DefaultMouseCursor = EMouseCursor::Default;

	
	// 创建一个FInputModeGameAndUI类型的对象InputModeData，用于配置输入模式
	FInputModeGameAndUI InputModeData;
	// 设置鼠标锁定模式为不锁定，允许鼠标在视口内自由移动
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// 设置在捕获鼠标输入时不隐藏光标
	InputModeData.SetHideCursorDuringCapture(false);
	// 将配置好地输入模式应用到当前上下文中
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent); //获取到增强输入组件
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move); //绑定移动事件

	// 绑定Shift按键事件
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);

}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	//方向控制，如果阻止了按住事件，将不再执行
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputHold))
	{
		return;
	}
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>(); //获取输入操作的2维向量值
	const FRotator Rotation = GetControlRotation(); //获取控制器旋转
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f); //通过控制器的垂直朝向创建一个旋转值，忽略上下朝向和左右的朝向
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //获取世界坐标系下向前的值，-1到1
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //获取世界坐标系下向右的值，-1到1
	
	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	//处理判断按下事件是否被阻挡
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
	/**
	* 在鼠标按下事件中，判断传入的Tag是否为鼠标左键事件，在内部判断点击的是否为敌人，
	* 如果不是敌人，那就是地面，如果ThisActor有值，那就是点击到了敌人。
	* TargetingStatus为记录当前按下时，目标是敌人、非敌人的物品、空。
	* bAutoRunning 为是否自动寻路中，在鼠标按下，将自动关闭此项，在鼠标抬起时再重新计算是否需要自动移动。
	* FollowTime用于统计按下时间，来去顶当前操作为点击还是长按。
	 */
	if (InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		// if (IsValid(ThisActor))
		// {
		// 	if(ThisActor->Implements<UEnemyInterface>())
		// 	{
		// 		// 继承敌人接口，目标为敌人
		// 		TargetingStatus = ETargetingStatus::TargetingEnemy;
		// 	}
		// 	else
		// 	{
		// 		// 无敌人接口，基本为场景静态物体
		// 		TargetingStatus = ETargetingStatus::TargetingNonEnemy;
		// 	}
		// }else
		// {
		// 	// 目标不存在，设置为无目标状态
		// 	TargetingStatus = ETargetingStatus::NotTargeting;
		// }
		TargetingStatus = IsValid(ThisActor) ?
							(ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy /*继承敌人接口，目标为敌人*/: ETargetingStatus::TargetingNonEnemy) /*无敌人接口，基本为场景静态物体*/
							: ETargetingStatus::NotTargeting; // 目标不存在，设置为无目标状态
		//bTargeting = ThisActor != nullptr; //ThisActor为鼠标悬停在敌人身上才会有值
		bAutoRunning = false;
		FollowTime = 0.0f; //重置统计的时间
	}
	// 调用ASC内创建的键位按下事件
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//处理判断抬起事件是否被阻挡
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());

	if (!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	
	//如果鼠标拾取状态为非敌人，并且没有按住Shift键，将进入寻路逻辑
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		if (const APawn* ControlledPawn = GetPawn(); FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (IsValid(ThisActor) && ThisActor->Implements<UHighlightInterface>())
			{
				IHighlightInterface::Execute_SetMoveToLocation(ThisActor, CachedDestination);
			}else if(GetASC() && !GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputPressed))
			{
				// 处理判断按下事件是否被阻挡
				// 生成点击奶瓜粒子
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
			
			// 如果找到到目标位置的路径
			if(UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				// 清空样条曲线上的点
				Spline->ClearSplinePoints();
				// 遍历路径上的点
				for (const FVector& Point : NavPath->PathPoints)
				{
					// 将路径上的点添加到样条曲线上
					Spline->AddSplinePoint(Point, ESplineCoordinateSpace::World);
					//DrawDebugSphere(GetWorld(), Point, 10.f, 8, FColor::Green, false, 5.f);
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					//自动寻路将最终目的地设置为导航的终点，方便停止导航
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				}
				bAutoRunning = true; //设置当前正常自动寻路状态，将在tick中更新位置
			}

		}
		FollowTime = 0.0f;
		//bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//通过标签阻止悬停事件的触发
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputHold))
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Yellow, *InputTag.ToString());
	if (!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
	{
		//点击敌人目标，将攻击敌人
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}else
	{
		// 没有选中敌人
		FollowTime += GetWorld()->GetDeltaSeconds(); //统计悬停时间来判断是否为点击
		// 将命中点的位置赋值给缓存目标
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;
		if(APawn* ControlledPawn = GetPawn())
		{
			/*
			 * 通过目标位置减去角色所在位置，就可以得到一个朝向，
			 * GetSafeNormal()为归一化向量，然后使用AddMovementInput去移动角色
			 */
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	//判断当前事件是否被阻挡，如果事件被阻挡，则清除相关内容
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_CursorTrace))
	{
		UnHighlightActor(LastActor);
		UnHighlightActor(ThisActor);
		ThisActor = nullptr; 
		LastActor = nullptr;
		return;
	}
	// 如果当前处于魔法范围显示阶段,将忽略场景中的角色
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_Target : ECC_Visibility;
                        
	// 获取光标下的命中结果，使用可见性碰撞检测，不进行碰撞检测确认
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit) return; //如果未命中直接返回
	
	LastActor = ThisActor;
	//获取拾取的Actor，判断Actor是否继承高亮接口
	if(IsValid(CursorHit.GetActor()) && CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor = CursorHit.GetActor();
	}
	else
	{
		ThisActor = nullptr;
	}
	
	// 如果两次拾取的目标不同，将修改高亮目标
	if(ThisActor != LastActor)
	{
		HighlightActor(ThisActor);
		UnHighlightActor(LastActor);
	}
}

void AAuraPlayerController::HighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(InActor);
	}
}

void AAuraPlayerController::UnHighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor(InActor);
	}
}

FHitResult AAuraPlayerController::GetCursorHit()
{
	return CursorHit;
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}


