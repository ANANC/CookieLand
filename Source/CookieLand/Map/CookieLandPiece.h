// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.h"
#include "GameplayTagContainer.h"
#include "CookieLandPiece.generated.h"

class UCookieLandPiece;
class UCookieLandMapShowDirector;
class UBoxComponent;
class UTimelineComponent;
class UPrimitiveComponent;
class UCookieLandPerceptualObjectComponent;
class UCookieLandBasePieceAction;
class ACookieLandMapBuildActor;

UCLASS(Blueprintable,BlueprintType)
class COOKIELAND_API ACookieLandPieceActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (DisplayName = "进入地块检测"))
	TObjectPtr<UBoxComponent> EnterBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TimeLine"))
	TObjectPtr<UTimelineComponent> TimelineComponent;
protected:

	bool bInit = false;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation PieceOrientation;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr< UCookieLandPiece> CookieLandPiece = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite)
	UCookieLandMapShowDirector* MapShowDirector;

public:

	ACookieLandPieceActor(const FObjectInitializer& Initializer);

	virtual void BeginDestroy() override;

	virtual void Init(UCookieLandPiece* InCookieLandPiece);
	virtual void UnInit();

	bool GetIsInit();

public:
	void SetMapShowDirector(UCookieLandMapShowDirector* InMapShowDirector);

	FCookieLandPieceLocator GetLocator() const;

public:
	UTimelineComponent* GetTimelineComponent() { return TimelineComponent.Get(); }
public:
	// 更新显示
	virtual void UpdateDisplay(){}

	// 进入地块
	virtual void EnterPiece(UCookieLandPerceptualObjectComponent* PerceptualObjectComponent);

protected:

	// 接收回调【MapShowDirector】TriggerPieceActorRenderUpdateEvent 更新地块刷新
	UFUNCTION()
	void ReceiveTriggerPieceActorRenderUpdateEventCallback(FCookieLandPieceLocator Locator);

	// 接收回调【EnterBox】OnComponentBeginOverlap 有物体进入回调
	UFUNCTION()
	void ReceiveEnterBoxOnComponentBeginOverlapCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};


UCLASS()
class COOKIELAND_API UCookieLandPiece : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite)
	FCookieLandPieceBuildInfo BuildInfo;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACookieLandPieceActor> PieceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite)
	ECookieLandPieceOrientation PieceOrientation;

	UPROPERTY(BlueprintReadWrite)
	FCookieLandPieceBaseAttribute BaseAttribute;

protected:
	bool bInit = false;

	UPROPERTY()
	TObjectPtr< ACookieLandMapBuildActor> MapBuildActor;

	UPROPERTY(VisibleAnywhere)
	TArray< UCookieLandBasePieceAction*> PieceActions;

	int AutoPieceActionId = 0;

	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer ActivingTagContainer;

public:
	virtual void Init(FCookieLandPieceBuildInfo InBuildInfo);
	virtual void UnInit();

	virtual void Active();

	// 设置地图构建实例
	void SetMapBuildActor(ACookieLandMapBuildActor* InMapBuildActor);

	// 获取地图构建实例
	ACookieLandMapBuildActor* GetMapBuildActor();

	// 设置实例Actor
	virtual void SetPieceActor(ACookieLandPieceActor* Instance);

	// 获取实例Actor
	ACookieLandPieceActor* GetPieceActor();

	// 设置实例类型
	void SetPieceActorType(TSubclassOf< ACookieLandPieceActor> PieceActorType);


	// 设置基础行为
	void SetBaseAttribute(FCookieLandPieceBaseAttribute InBaseAttribute);

	// 获取实例
	ACookieLandPieceActor* GetPieceAction();

	// 获取位置
	FCookieLandLocation GetPieceLocation();

	// 获取方位
	ECookieLandPieceOrientation GetPieceOrientation();

	// 获取坐标
	FCookieLandPieceLocator GetPieceLocator();

	// 获取基础行为
	FCookieLandPieceBaseAttribute GetBaseAttribute();

	// 获取构建信息
	FCookieLandPieceBuildInfo GetBuildInfo();

	// 添加地块行为
	void AddPieceAction(UCookieLandBasePieceActionData* PieceActionData);

	// 移除地块行为，根据类型判断
	void RemovePieceActionByType(TSubclassOf<UCookieLandBasePieceAction> PieceActionClassType);

	// 移除地块行为，根据Id
	void RemovePieceActionById(int InId);

	// 清理全部地块行为
	void ClearPieceActions();

	// 根据类型获取地块行为
	const UCookieLandBasePieceAction* GetConstPieceActionByType(TSubclassOf<UCookieLandBasePieceAction> PieceActionClassType);

	// 根据类型获取地块行为
	const UCookieLandBasePieceAction* GetConstPieceActionById(int InId);

	// 添加激活的GameplayTag
	void AddActiveGameplayTag(FGameplayTag InGameplayTag);

	// 移除激活的GameplayTag
	void RemoveActiveGameplayTag(FGameplayTag InGameplayTag);

	// 获取是否有激活的GameplayTag
	bool HasActiveGameplayTag(FGameplayTag InGameplayTag);

protected:
	// 根据类型获取地块行为
	 UCookieLandBasePieceAction* GetPieceActionByType(TSubclassOf<UCookieLandBasePieceAction> PieceActionClassType);

	// 根据类型获取地块行为
	UCookieLandBasePieceAction* GetPieceActionById(int InId);
};
