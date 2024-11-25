// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
#include "CookieLandMapShowDirector.generated.h"

class ACookieLandMapBuildActor;

UCLASS()
class COOKIELAND_API UCookieLandMapShowDirector : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map, meta = (DisplayName = "显示数据"))
	FCookieLandMapShowInfo MapShowInfo;

protected:
	UPROPERTY()
	ACookieLandMapBuildActor* BuildActor;

public:
	void SetBuildActor(ACookieLandMapBuildActor* InBuildActor);

public:
	// 获取能否显示
	bool GetEnableDisplay(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);
};
