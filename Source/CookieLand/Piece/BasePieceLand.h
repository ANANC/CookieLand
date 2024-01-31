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

	UPROPERTY()
	UPieceLandBoundInfo* BoundInfo;
	
	UPROPERTY(VisibleAnywhere)
	TArray<UPieceLocationInfo*> OccupyStates;
	
public:
	void CreateLand(FName levelName,class ULandDataAsset* landDA);

	void DestroyLand();
	
	bool GetEnableOccupyLocation(FPieceLocation location,int& OccupyId);
	
	bool RequestOccupyLocation(int Id,FPieceLocation location);

	bool RequestUnOccupyLocation(int Id,FPieceLocation location);

	FName GetLevelName();
	
	int GetInitialPieceId();
	
	UFUNCTION(BlueprintPure)
	UBasePiece* GetPieceById(int Id);
	
	UFUNCTION(BlueprintPure)
	FVector GetActorLocationById(int Id);
	
	UFUNCTION(BlueprintPure)
	FVector GetActorLocationByLocation(FPieceLocation location);

	bool GetPieceIdByLocation(FPieceLocation location,int& pieceId);
	
	bool IsInFinishLocation(int pieceId);

	UFUNCTION(BlueprintPure)
	bool IsFinishPieceId(int pieceId);

	UFUNCTION(BlueprintPure)
	bool RequestToNextLocation(int pieceId,EPieceDirection direction,int& nextPieceId);

	TSubclassOf<class ABasePieceActor> GetPieceInstanceActorClass(UBasePiece* piece);
	
	UFUNCTION(BlueprintPure)
	TArray<UBasePiece*> GetFloorPieces(int floor);
protected:
	UBasePiece* CreatePiece(UPieceBaseConfigData* pieceData);

	UPieceLocationInfo* GetLocationInfo(FPieceLocation location);


	TArray<UBasePiece*> GetDirectionPiecesByFloorPieces(TArray<UBasePiece*> floorPieces,FPieceLocation curLocation,EPieceDirection direction);

	UBasePiece* GetNearPieceByDirectionPieces(int pieceId,FPieceDistance distance,EPieceDirection direction);

	UBasePiece* GetNearPieceByUpOrDown(int pieceId,FPieceDistance distance,EPieceDirection direction);
};
