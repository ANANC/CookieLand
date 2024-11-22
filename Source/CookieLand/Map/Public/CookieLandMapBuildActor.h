// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CookieLandMapTypes.h"
#include "CookieLandMapBuildActor.generated.h"

class ACookieLandPieceActor;
class UCookieLandMapBuilder;
class UCookieLandMapActorGather;
class UCookieLandPiece;


UCLASS()
class COOKIELAND_API ACookieLandMapBuildActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACookieLandMapBuildActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

#if WITH_EDITORONLY_DATA
	static ACookieLandMapBuildActor* MapBuildActorInstance;
#endif

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map, meta = (DisplayName = "地图名"))
	FName MapName = "Default";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map, meta = (DisplayName = "地形数据"))
	FCookieLandMapBuildInfo MapBuildInfo;

protected:

	UPROPERTY()
	UCookieLandMapBuilder* MapBuilder;

	UPROPERTY()
	UCookieLandMapActorGather* MapActorGather;

public:
	UCookieLandMapBuilder* GetMapBuilder() { return MapBuilder; }
	UCookieLandMapActorGather* GetMapActorGather() { return MapActorGather; }
public:
	// 重新加载地形数据
	void ReloadMapBuildInfo();

	// 创建环境
	void CreateEnvironment();

	// 创建立方体
	void CreateCube(const FCookieLandLocation CubeLoaction);

	// 创建地块
	void CreatePiece(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 删除立方体
	void DeleteCube(const FCookieLandLocation CubeLoaction);

	// 删除地块
	void DeletePiece(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation);

	// 删除指定层的全部地块
	void DeleteTargetFloorAllPiece(int Floor);

	// 删除全部地块
	void DeleteAllPiece();

	// 更新全部地块实例坐标
	void UpdateAllPieceInstanceLocation();

	// 获取全部的地块信息
	TArray<FCookieLandPieceBuildInfo> GetAllPieceBuildInfo();

public:

	// [MapBuilder] 获取是否立方体已经占领
	bool GetIsCubeOccupyByLocation(const FCookieLandLocation MapCubeLocation);

	// [MapBuilder] 获取是否地块已经占领
	bool GetIsPieceOccupyByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// [MapActorGather] 获取地块
	UCookieLandPiece* GetPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation);

	// [MapActorGather] 切换地块的实例类型
	void ChangePieceActorType(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation, TSubclassOf< ACookieLandPieceActor> InPieceActorType);

	// [MapBuilder] 获取全部占领的层
	TArray<int> GetAllOccupyFloor();

};
