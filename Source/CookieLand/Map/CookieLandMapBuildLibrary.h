// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CookieLandMapTypes.h"
#include "CookieLandMapBuildLibrary.generated.h"

class ACookieLandPieceActor;
class UCookieLandPerceptualObjectSubsystem;
class UCookieLandMapSubsystem;


UCLASS()
class COOKIELAND_API UCookieLandMapBuildLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// 获取感知者系统
	UFUNCTION(BlueprintPure)
	static UCookieLandPerceptualObjectSubsystem* GetPerceptualObjectSubsystem();

	// 获取地块系统
	UFUNCTION(BlueprintPure)
	static UCookieLandMapSubsystem* GetMapSubsystem();

public:

	// 直接从地形表中读取地形原始数据
	UFUNCTION(BlueprintPure)
	static bool GetSourceMapBuildDataTable(FName MapName, FCookieLandMapBuildDataTableRow& OutMapBuildDataTableRow);

	// 获取修正后的地形数据，根据默认数据进行填充。
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

	// 计算地块的旋转
	UFUNCTION(BlueprintPure)
	static FQuat CalculatePieceActorInstanceRotation(const FCookieLandMapBuildInfo& InMapBuildInfo, const ECookieLandPieceOrientation& PieceOrientation);

	// 创建正方体构建信息
	UFUNCTION(BlueprintPure)
	static TArray< FCookieLandPieceBuildInfo> CrateCubeBuildInfo(const FCookieLandLocation& PieceLocation);

	// 创建地块构建信息
	UFUNCTION(BlueprintPure)
	static FCookieLandPieceBuildInfo CratePieceBuildInfo(const FCookieLandLocation& PieceLocation, const ECookieLandPieceOrientation& PieceOrientation);

	// 从感知者表中读取感知数据
	UFUNCTION(BlueprintPure)
	static bool GetPerceptualObjectPerceptionDataTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionDataTableRow& OutPerceptualObjectPerceptionDataTableRow);

	// 从感知表中读取感知配置
	static bool GetPerceptualObjectPerceptionInfoFromTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionInfo& OutPerceptualObjectPerceptionInfo);

	// 获取视角对应的方向列表
	static TArray<ECookieLandPieceOrientation> GetMapAgnleViewOrientations(ECookieLandMapAngleViewType MapAngleViewType, const ECookieLandPieceOrientation& PieceOrientation);

	// 获取范围坐标
	static void GetRectPieceLocators(TArray< FCookieLandPieceLocator>& OutLocators, FCookieLandPieceLocator CenterLocator, int FloorDistance, bool bNeedOtherFloorLocator, int RoundDistance);

	// 获取自身能否往上移动
	static bool GetEnableMoveUpByMyself(const FCookieLandPieceBaseAction& BaseAction);

	// 获取自身能否往上移动
	static bool GetEnableMoveDownByMyself(const FCookieLandPieceBaseAction& BaseAction);

	// 从感知者表中读取感知者数据
	UFUNCTION(BlueprintPure)
	static bool GetPerceptionInfoDataTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionDataTableRow& OutPerceptionInfoDataTableRow);

};
