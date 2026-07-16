// Huanyu,miaomiaomiao,nekoneko


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Aura/AuraLogChannels.h"
//#include "Aura/AuraLogChannels.h"


AAuraProjectile::AAuraProjectile()
{
	// 设置该Actor是否可以每帧更新
	PrimaryActorTick.bCanEverTick = false;
	// 设置该Actor是否可以复制
	bReplicates = true;//服务器负责计算并更新Actor的状态，然后通过网络将这些更新复制到所有连接的客户端上。

	//初始化碰撞体
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere); //设置其为根节点，
	Sphere->SetCollisionObjectType(ECC_PROJECTILE); //设置发射物的碰撞类型
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //设置其只用作查询使用
	Sphere->SetCollisionResponseToChannels(ECR_Ignore); //设置其忽略所有碰撞检测
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); //设置其与世界动态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); //设置其与世界静态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //设置其与Pawn类型物体产生重叠事件

	//创建发射组件
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f; //设置初始速度
	ProjectileMovement->MaxSpeed = 550.f; //设置最大速度
	ProjectileMovement->ProjectileGravityScale = 0.f; //设置重力影响因子，0为不受影响

}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 设置此物体的存在时间
	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	// 添加一个音效，并附加到根组件上面，在技能移动时，声音也会跟随移动
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());


}

void AAuraProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	//如果没有权威性，并且bHit没有修改为true，证明当前没有触发Overlap事件，在销毁前播放击中特效
	if (!bHit && !HasAuthority()) OnHit();
	if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(DestroyHandle);
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 有效性检查：排除无效碰撞（如自身或友军）
	if (!IsValidOverlap(OtherActor)) return;
	// 触发击中效果：播放音效/特效并标记已击中
	if (!bHit) OnHit();
	// 服务器端处理：应用伤害效果并销毁投射物
	if(HasAuthority())
	{
		// 为目标应用GE
		if (UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{

			//死亡冲击的力度和方向
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			
			//判断攻击是否触发击退
			if(const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance)
			{
				// 原本👇
				//将技能攻击的朝向向上偏转，基于向右的方向向量
				// const FVector KnockbackDirection = GetActorForwardVector().RotateAngleAxis(-45.f, GetActorRightVector());
				// DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
				// 👇是固定角度，👆非固定
				//获取到攻击对象和目标的朝向，并转换成角度
				FRotator Rotation = (OtherActor->GetActorLocation() - GetActorLocation()).Rotation();
				Rotation.Pitch = 45.f; //设置击退角度垂直45度
				const FVector ToTarget = Rotation.Vector();
				DamageEffectParams.KnockbackForce = ToTarget * DamageEffectParams.KnockbackForceMagnitude;
				//UE_LOG(LogAura, Warning, TEXT("KnockbackDirection: %s"), *KnockbackDir.ToString())

				
			}
			
			// 绑定目标ASC并应用伤害效果
			DamageEffectParams.TargetAbilitySystemComponent = TargetAsc;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		
		// 在服务器上执行的操作
		Destroy();

	}
	else
	{
		// 在客户端上执行的操作
		//如果对actor没有权威性，将bHit设置为true，证明当前已经播放了击中特效
		bHit = true;
	}
}

bool AAuraProjectile::IsValidOverlap(AActor* OtherActor)
{
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr) return false;
	if (AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor())
	{
		if (UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// 源头角色与碰撞角色相同，视为无效碰撞（防止攻击自己）
			if (SourceAvatarActor == OtherActor) return false;
			// 双方是友军关系，视为无效碰撞（友军免伤）
			if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor)) return false;

			
			if (TargetAsc->HasMatchingGameplayTag(FMyGameplayTags::Get().Abilities_Passive_HaloOfProtection))
			{
				// 碰撞角色有Abilities_Passive_HaloOfProtection标签，则销毁当前特效
				// TODO:下面是我做护盾防止飞弹消失太快
				SetLifeSpan(0.0f); // 清除定时器，Actor不再自动销毁
				// 播放特效
				OnHit();
				// 蓝图调用，关闭特效
				OnDestroy();
				// 关闭碰撞
				Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				// 使用Lambda表达式绑定无参Destroy调用
				GetWorld()->GetTimerManager().SetTimer(
					DestroyHandle,
					[this]() { this->Destroy(); },
					DestroyDelay,
					false
				);
				return false;
			}
		}

	}
	//所有检查通过，视为有效碰撞
	return true;
}

void AAuraProjectile::OnHit()
{
	//播放音效
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	//播放粒子特效
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	//将音乐停止后会自动销毁
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}



