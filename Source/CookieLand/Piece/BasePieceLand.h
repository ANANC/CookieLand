// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PieceTypes.h"
#include "BasePieceLand.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UBasePieceLand : public UObject
{
	GENERATED_BODY()

protected:
	
	FName LevelName;
	
	UPROPERTY()
	class ULandDataAsset* LandDataAsset;

	UPROPERTY(VisibleAnywhere)
	TArray<class UBasePiece*> Pieces;
	
	UPROPERTY()
	TMap<int,class UBasePiece*> PieceMap;
	
	UPROPERTY(VisibleAnywhere)
	TArray<UPieceLocationInfo*> OccupyStates;
	
public:
	void CreateLand(FName levelName,class ULandDataAsset* landDA);

	void DestroyLand();
	
	bool GetEnableOccupyLocation(FPieceLocation location,int& OccupyId);
	
	bool RequestOccupyLocation(int Id,FPieceLocation location);

	bool RequestUnOccupyLocation(int Id,FPieceLocation location);

	UBasePiece* GetPieceById(int Id);

	FVector GetActorLocationById(int Id);
	
	FVector GetActorLocationByOccupyLocation(FPieceLocation location);

	bool GetPieceIdByLocation(FPieceLocation location,int& pieceId);
	
	bool IsInFinishLocation(FPieceLocation location);
	
	bool RequestToNextLocation(FPieceLocation curLocation,EPieceDirection direction,FPieceLocation& newLocation);
protected:
	UBasePiece* CreatePiece(UPieceBaseConfigData* pieceData);

	UPieceLocationInfo* GetLocationInfo(FPieceLocation location);

	TArray<UBasePiece*> GetFloorPieces(int floor);

	TArray<UBasePiece*> GetDirectionPiecesByFloorPieces(TArray<UBasePiece*> floorPieces,FPieceLocation curLocation,EPieceDirection direction);

	UBasePiece* GetNearPieceByDirectionPieces(TArray<UBasePiece*> directionPieces,FPieceLocation curLocation,FPieceDistance Distance,EPieceDirection direction);
};
