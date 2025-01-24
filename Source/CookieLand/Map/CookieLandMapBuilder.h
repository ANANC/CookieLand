// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.h"
#include "CookieLandMapBuilder.generated.h"

UCLASS()
class UCookieLandMapCubeInfo : public UObject
{
	GENERATED_BODY()

public:
	// 坐标
	FCookieLandLocation Location;

	// 当前占领状态
	bool bOccupy = false;

	// 方位占领
	TMap<ECookieLandPieceOrientation, bool> OrientationOccupys;

public:
	void OccupyPiece(const ECookieLandPieceOrientation PieceOrientation);

	void ReleasePiece(const ECookieLandPieceOrientation PieceOrientation);

	void UpdateOccupyState();
};


UCLASS()
class UCookieLandMapRangeInfo : public UObject
{
	GENERATED_BODY()

public:
	// 方向 [左右->左，前后->前，上下->上]
	ECookieLandPieceOrientation Orientation;

	// 坐标 [左右->[y,Floor],前后->[x,Floor],上下->[x,y]]
	FCookieLandLocation Location;

	// 整个方向上的地块
	TArray<FCookieLandLocation> PieceLocastions;

	// 强制连接关系
	TArray< FCookieLandOrientationLinkInfo> ForceLineInfos;

public:
	void Init(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);
	bool GetIsConformanceRange(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);
	void AddPiece(const FCookieLandLocation PieceLocation);
	void RemovePiece(const FCookieLandLocation PieceLocation);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCookieLandMapLocatorOccupyStateChangeEvent, FCookieLandPieceLocator, Locator, bool, Occupy);

UCLASS()
class COOKIELAND_API UCookieLandMapBuilder : public UObject
{
	GENERATED_BODY()
	
public:
	FCookieLandMapLocatorOccupyStateChangeEvent MapLocatorOccupyStateChangeEvent;

protected:
	UPROPERTY()
	TArray< UCookieLandMapCubeInfo*> MapCubeInfos;

	UPROPERTY()
	TMap<FVector, UCookieLandMapCubeInfo*> Location2MapCubeInfos;

	// 基于自身坐标和方向的记录，Locator只会有一份记录
	// 如[x:0,y:0,floor:0][上], 会被记录到[上][x:0,y:0]
	UPROPERTY()
	TArray< UCookieLandMapRangeInfo*> MapRangeInfos;

	// 全方向记录，Locator会有多分记录
	// 如[x:0,y:0,floor:0][上], 会被记录到[上][x:0,y:0],[前][x:0,floor:0],[左][y:0,floor:0]
	UPROPERTY()
	TArray< UCookieLandMapRangeInfo*> MapOmnibearingRangeInfos;

public:
	void Init();
	void UnInit();

public:

	// 获取地形是否为空
	bool GetMapIsEmpty();

	// 获取是否立方体已经占领
	bool GetIsCubeOccupyByLocation(const FCookieLandLocation MapCubeLocation);

	// 获取是否地块已经占领
	bool GetIsPieceOccupyByLocator(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 占领地块
	bool OccupyPieceByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 释放地块
	bool ReleasePieceByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取全部占领地块
	TArray< FCookieLandPieceLocator> GetAllOccupyPiece();

	// 获取特定层的全部占领地块
	TArray< FCookieLandPieceLocator> GetAllOccupyPieceByTargetFloor(int InFloor);

	// 获取全部占领的层
	TArray<int> GetAllOccupyFloor();

	// 获取最近的地块坐标
	bool GetNearestPieceLocator(const FCookieLandPieceLocator SourceLocator,ECookieLandPieceOrientation Orientation, FCookieLandPieceLocator& OutNearestLocator);
public:

	// 地块强制连接
	bool PieceForceLine(const FCookieLandLocation RequsetPieceLocation, const FCookieLandLocation TryLinePieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 地块删除强制连接
	bool PieceDeleteForceLine(const FCookieLandLocation RequsetPieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取是否在强制连接的内部状态且【非处于边缘】
	bool GetWhetherInternalStateOfForcedLineAndNotAtEdge(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);
	bool GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(const FCookieLandOrientationLinkInfo& LineInfo, const FCookieLandLocation PieceLocation);

	// 获取是否在强制连接的内部状态且【处于边缘】
	bool GetInEdgeByLineInfo(const FCookieLandOrientationLinkInfo& LineInfo, const FCookieLandLocation PieceLocation);

	// 获取当前所在的强制连接范围
	bool GetForceLineInfo(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation,FCookieLandOrientationLinkInfo& OutLineInfo);

	// 获取基于当前坐标最靠近的强制连接
	bool GetNearestForceLineInfo(const ECookieLandPieceOrientation PieceOrientation, const FCookieLandLocation StartPieceLocation, int Distance, FCookieLandOrientationLinkInfo& OutLineInfo);

protected:

	// 创建/获取立方体
	UCookieLandMapCubeInfo* CreateOrGetMapCubeInfo(const FCookieLandLocation MapCubeLocation);

	// 获取立方体
	UCookieLandMapCubeInfo* GetMapCubeInfo(const FCookieLandLocation MapCubeLocation);

	// 创建/获取地图范围
	UCookieLandMapRangeInfo* CreateOrGetMapRangeInfo(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 创建/获取地图全范围
	TArray<UCookieLandMapRangeInfo*> CreateOrGetMapOmnibearingRangeInfos(const FCookieLandLocation PieceLocation);

	// 更新地图范围
	void UpdateMapRange(bool bIsAdd, const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 地图范围尝试移除
	void MapRangeInfoTryRemove(UCookieLandMapRangeInfo* MapRangeInfo, const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);

public:

	// 执行感知者请求强制连接信息
	UFUNCTION()
	FCookieLandOrientationLinkInfo ExecutePerceptualObjectFindForceLinkInfoEventCallback(FCookieLandPieceLocator Locator);
};
