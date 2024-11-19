// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
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

	// 方位连接
	TMap< ECookieLandPieceOrientation, FCookieLandLocation> OrientationForceLinks;

	// 当前强制关联状态
	bool bForceLine = false;
public:
	void OccupyPiece(const ECookieLandPieceOrientation PieceOrientation);

	void ReleasePiece(const ECookieLandPieceOrientation PieceOrientation);

	void UpdateOccupyState();

	void AddForceLine(const ECookieLandPieceOrientation PieceOrientation, const FCookieLandLocation LineLocation);

	void DeleteForceLine(const ECookieLandPieceOrientation PieceOrientation);

	void UpdateForceLineState();
};


USTRUCT(BlueprintType)
struct FCookieLandOrientationLinkRangeInfo
{
	GENERATED_USTRUCT_BODY()

public:
	// 范围[右，上，前]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation Max_PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation Max_PieceOrientation;

	// 范围[左，下，后]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation Min_PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation Min_PieceOrientation;

	// 距离
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Distance;
};

UCLASS()
class UCookieLandMapBuilder : public UObject
{
	GENERATED_BODY()
	

protected:
	UPROPERTY()
	TArray< UCookieLandMapCubeInfo*> MapCubeInfos;

	UPROPERTY()
	TMap<FVector, UCookieLandMapCubeInfo*> Location2MapCubeInfos;

public:
	void Init();
	void UnInit();

public:

	// 获取地形是否为空
	bool GetMapIsEmpty();

	// 获取是否立方体已经占领
	bool GetIsCubeOccupyByLocation(const FCookieLandLocation MapCubeLocation);

	// 获取是否地块已经占领
	bool GetIsPieceOccupyByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

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

public:

	// 获取地块强制连接的地块
	bool GetPieceForceLineLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation, FCookieLandPieceLocator& OutLineLocator);

	// 地块强制连接
	bool PieceForceLine(const FCookieLandLocation RequsetPieceLocation, const FCookieLandLocation TryLinePieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 地块删除强制连接
	bool PieceDeleteForceLine(const FCookieLandLocation RequsetPieceLocation, const FCookieLandLocation TryLinePieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 是否在连接状态
	bool GetIsInLineState(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取是否在连接的内部状态
	bool GetIsInLineRangeState(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取当前所在的连接范围（包含强制连接，自动连接）
	bool GetLineRangeInfo(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation,FCookieLandOrientationLinkRangeInfo& OutLineRangeInfo);
protected:

	// 创建/获取立方体
	UCookieLandMapCubeInfo* CreateOrGetMapCubeInfo(const FCookieLandLocation MapCubeLocation);

	// 获取立方体
	UCookieLandMapCubeInfo* GetMapCubeInfo(const FCookieLandLocation MapCubeLocation);

};
