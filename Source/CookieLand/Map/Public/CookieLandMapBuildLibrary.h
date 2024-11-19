// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CookieLandMapTypes.h"
#include "CookieLandMapBuildLibrary.generated.h"

class ACookieLandPieceActor;


UCLASS()
class COOKIELAND_API UCookieLandMapBuildLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// 直接从表中读取原始数据
	UFUNCTION(BlueprintPure)
	static bool GetSourceMapBuildDataTable(FName MapName, FCookieLandMapBuildDataTableRow& OutMapBuildDataTableRow);

	// 获取修正后的地形数据。根据默认数据进行填充。
	UFUNCTION(BlueprintPure)
	static bool GetAmendMapBuildDataTable(FName MapName, FCookieLandMapBuildDataTableRow& OutMapBuildDataTableRow);

	// 创建地块实例
	UFUNCTION(BlueprintPure)
	static ACookieLandPieceActor* CreatePieceActorInstanceByMapName(const UObject* ObjectInstance, FName MapName,const FCookieLandPieceBuildInfo& InBuildInfo);

	// 创建地块实例
	UFUNCTION(BlueprintPure)
	static ACookieLandPieceActor* CreatePieceActorInstanceByBuildInfo(const UObject* ObjectInstance, const FCookieLandMapBuildInfo& InMapBuildInfo, const FCookieLandPieceBuildInfo& InBuildInfo);

	// 计算地块坐标
	UFUNCTION(BlueprintPure)
	static FVector CalculatePieceActorInstanceLocation(const FCookieLandMapBuildInfo& InMapBuildInfo,const FCookieLandLocation& PieceLocation, const ECookieLandPieceOrientation& PieceOrientation);

	UFUNCTION(BlueprintPure)
	static FQuat CalculatePieceActorInstanceRotation(const FCookieLandMapBuildInfo& InMapBuildInfo, const ECookieLandPieceOrientation& PieceOrientation);

	// 创建正方体构建信息
	UFUNCTION(BlueprintPure)
	static TArray< FCookieLandPieceBuildInfo> CrateCubeBuildInfo(const FCookieLandLocation& PieceLocation);

	// 创建地块构建信息
	UFUNCTION(BlueprintPure)
	static FCookieLandPieceBuildInfo CratePieceBuildInfo(const FCookieLandLocation& PieceLocation, const ECookieLandPieceOrientation& PieceOrientation);
};
