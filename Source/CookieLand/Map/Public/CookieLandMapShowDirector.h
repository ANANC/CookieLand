// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
#include "Runtime/Core/Public/Delegates/DelegateCombinations.h"
#include "CookieLandMapShowDirector.generated.h"

class ACookieLandMapBuildActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggerPieceActorRenderUpdateEvent, FCookieLandPieceLocator, Locator);

UCLASS()
class COOKIELAND_API UCookieLandMapShowDirector : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map, meta = (DisplayName = "显示数据"))
	FCookieLandMapShowInfo MapShowInfo;

public:

	// 通知地块Acotr更新渲染
	FTriggerPieceActorRenderUpdateEvent TriggerPieceActorRenderUpdateEvent;

protected:
	UPROPERTY()
	ACookieLandMapBuildActor* BuildActor;

	bool bDisplayAll = false;
public:
	void SetBuildActor(ACookieLandMapBuildActor* InBuildActor);

	void SetDisplayAll(bool bValue);

public:
	// 获取能否显示【总计算】
	bool GetEnableDisplay(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取能否显示【根据显示类型】
	bool GetEnableDisplaySwitchMapShowType(FCookieLandPieceLocator MainLocator, const ECookieLandPieceOrientation PieceOrientation);

	// 监听感知者移动回调
	UFUNCTION()
	void ReceivePerceptualObjectLocatorChangeEventCallback(int Id, FCookieLandPieceLocator OldLocator, FCookieLandPieceLocator NewLocator);
};
