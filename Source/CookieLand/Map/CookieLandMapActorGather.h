// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.h"
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
class COOKIELAND_API UCookieLandMapActorGather : public UObject
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
	UCookieLandPiece* AddPiece(const FCookieLandPieceBuildInfo PieceBuildInfo);

	// 移除地块
	bool RemovePiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取地块
	UCookieLandPiece* GetPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 获取全部地块
	TArray<UCookieLandPiece*> GetAllPieces();

protected:

	// 创建或者创建地块
	UCookieLandPiece* CreatePieceToMapCubeInfo(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 创建地块实例
	ACookieLandPieceActor* CreatePieceAction(const FCookieLandPieceBuildInfo PieceBuildInfo);

	// 从立方体中移除地块数据
	void RemovePieceFromMapCubeInfo(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);
};
