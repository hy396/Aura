// Huanyu,miaomiaomiao,nekoneko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointCollection.generated.h"

UCLASS()
class AURA_API APointCollection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APointCollection();
	
	/**
	 * 获取到actor当前世界位置下的多个数量的位置
	 * @param GroundLocation 地面位置
	 * @param NumPoints 需要返回的数量最大10个
	 * @param YawOverride 设置actor旋转的值
	 * @return 
	 */
	UFUNCTION(BlueprintPure)
	TArray<USceneComponent*> GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride = 0.f);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * 初始化时使用，减少重复代码，创建多个点并添加到根组件下面
	 * @param Pt 创建的场景组件引用
	 * @param Name 场景组件名称
	 */
	void CreateSceneComponent(TObjectPtr<USceneComponent>& Pt, FName Name);

	//存储下面场景组件的数组
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<USceneComponent*> ImmutablePts;

	//场景的根组件，作为检测位置的根
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_2;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_3;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_4;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_5;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_6;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_7;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_8;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_9;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_10;
                        
};
