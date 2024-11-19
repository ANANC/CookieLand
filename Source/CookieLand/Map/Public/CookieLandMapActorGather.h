// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
#include "CookieLandMapActorGather.generated.h"

class UCookieLandPiece;
class ACookieLandPieceActor;

UCLASS()
class UCookieLandMapCubeActorInfo : public UObject
{
	GENERATED_BODY()

public:
	// 坐标
	FCookieLandLocation Location;

	// 方位地块
	UPROPERTY()
	TMap<ECookieLandPieceOrientation, UCookieLandPiece*> OrientationPieces;
};


UCLASS()
class UCookieLandMapActorGather : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map, meta = (DisplayName = "地形数据"))
	FCookieLandMapBuildInfo MapBuildInfo;

protected:
	UPROPERTY()
	TArray<UCookieLandPiece*> Pieces;

	UPROPERTY()
	TMap<FVector, UCookieLandMapCubeActorInfo*> Locaiton2Pieces;


public:

	// 获取是否地块已经存在
	bool GetIsPieceExistByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 创建地块
	bool AddPiece(const FCookieLandPieceBuildInfo PieceBuildInfo);

	// 移除地块
	bool RemovePiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 切换地块的实例类型
	void ChangePieceActorType(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation, TSubclassOf< ACookieLandPieceActor> InPieceActorType);

	// 获取地块
	UCookieLandPiece* GetPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取全部地块
	TArray<UCookieLandPiece*> GetAllPieces();

	// 给地块创建实例（如果已经存在不会二次创建）
	bool TryCreatePieceActorToPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);
protected:

	// 创建或者创建地块
	UCookieLandPiece* CreatePieceToMapCubeInfo(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 创建地块实例
	ACookieLandPieceActor* CreatePieceAction(const FCookieLandPieceBuildInfo PieceBuildInfo);

	// 从立方体中移除地块数据
	void RemovePieceFromMapCubeInfo(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);
};