// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Components/WidgetComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Interaction/HighlightInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	//服务器初始化ASC
	virtual void PossessedBy(AController* NewController) override;
	//客户端初始化ASC
	virtual void OnRep_PlayerState() override;
	
	/*	Enemy Interface */
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	// 设置敌人等级
	virtual void SetLevel_Implementation(const int32 InLevel) override { Level = InLevel; }
	/* end Enemy Interface*/

	/*	Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die_Implementation(const FVector& DeathImpulse) override;
	/* end Combat Interface*/

	/*	Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/* end Highlight Interface*/

	//使用overlay的委托来绑定，用于更改进度条生命值。所以就不用创建新的委托
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	// 当前是否处于被攻击状态
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false; 

	// 设置死亡后的存在时间
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat",
		meta=(DisplayName="死亡后存在时间",
		Tooltip="设置该角色的死亡后可以在世界存在的时间"))
	float LifeSpan = 10.f; 

	// 敌人需要攻击的目标
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;


protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	//眩晕标签变动后的回调
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults",
	// 	meta=(DisplayName="职业",
	// 	Tooltip="设置该角色的职业"))
	// ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
	
	//生成战利品
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();

};
