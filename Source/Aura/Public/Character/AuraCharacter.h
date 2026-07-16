// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Character/AuraCharacterBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	//服务器初始化ASC
	virtual void PossessedBy(AController* NewController) override;
	//客户端初始化ASC
	virtual void OnRep_PlayerState() override;
	
	/* IPlayerInterface战斗接口 */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;

	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;

	virtual TSubclassOf<UGameplayEffect> GetSecondaryAttributes_Implementation() override;
	virtual TSubclassOf<UGameplayEffect> GetVitalAttributes_Implementation() override;

	/* IPlayerInterface战斗接口 结束 */

	/*	Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual USphereComponent* GetDunSphereComponent_Implementation() override;
	virtual void SetDunSphereComponent_Implementation(bool bInCollision) override;
	virtual void Die_Implementation(const FVector& DeathImpulse) override;
	/* end Combat Interface*/

	//角色死亡后持续时间，用于表现角色死亡
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DeathTime = 5.f;

	//声明一个计时器，用于角色死亡后一定时间处理后续逻辑
	FTimerHandle DeathTimer;	
	
	// 升级特效
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
	
	// AT力场
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> DunSphere;
	
	virtual void OnRep_Stunned() override;
	virtual void OnRep_Burned() override;

	//角色加载存档保存的数值
	void LoadProgress() const;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;


	virtual void InitAbilityActorInfo() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};


