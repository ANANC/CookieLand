// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PieceTypes.h"
#include "BasePieceLand.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLandLocationOccupyStateChangeEvent, int, Id, FPieceLocation, location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLandLocationUnOccupyStateChangeEvent, int, Id, FPieceLocation, location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLandDestroyEvent);

/**
 * 
 */
UCLASS()
class COOKIELAND_API UBasePieceLand : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLandDestroyEvent LandDestroyBeforeEvent;
	
	UPROPERTY(BlueprintAssignable)
	FLandDestroyEvent LandDestroyEvent;
	
	UPROPERTY(BlueprintAssignable)
	FLandLocationOccupyStateChangeEvent LandLocationOccupyStateChangeEvent;
	
	UPROPERTY(BlueprintAssignable)
	FLandLocationUnOccupyStateChangeEvent LandLocationUnOccupyStateChangeEvent;
protected:
	
	FName LevelName;
	
	UPROPERTY(BlueprintReadOnly)
	class ULandDataAsset* LandDataAsset;

	UPROPERTY(VisibleAnywhere)
	TArray<class UBasePiece*> Pieces;
	
	UPROPERTY()
	TMap<int,class UBasePiece*> PieceMap;

	UPROPERTY()
	UPieceLandBoundInfo* BoundInfo;
	
	UPROPERTY(VisibleAnywhere)
	TArray<UPieceLocationInfo*> OccupyStates;
	
	int actionAutoId{1};
	
public:
	void CreateLand(FName levelName,class ULandDataAsset* landDA);

	void DestroyLand();
	
	UFUNCTION(BlueprintCallable)
	void DeletePieceById(int pieceId);
	
	bool GetEnableOccupyLocation(FPieceLocation location,int& OccupyId);
	
	bool RequestOccupyLocation(int pieceId,FPieceLocation location);

	bool RequestUnOccupyLocation(int pieceId,FPieceLocation location);
	
	UFUNCTION(BlueprintPure)
	bool CreateActionToPiece(FPieceActionHandle& handle,int pieceId,class UPieceBaseActionConfigData* actionData);
	
	UFUNCTION(BlueprintCallable)
	void DeleteActionByPiece(FPieceActionHandle handle);
	
	UFUNCTION(BlueprintPure)
	FName GetLevelName();
	
	int GetInitialPieceId();
	
	UFUNCTION(BlueprintPure)
	UBasePiece* GetPieceById(int pieceId);
	
	UFUNCTION(BlueprintPure)
	FPieceLocation GetLocationById(int pieceId);
	
	UFUNCTION(BlueprintPure)
	FVector GetActorLocationById(int pieceId);
	
	UFUNCTION(BlueprintPure)
	FVector GetActorLocationByLocation(FPieceLocation location);

	UFUNCTION(BlueprintPure)
	bool GetPieceIdByLocation(FPieceLocation location,int& pieceId);
	
	bool IsInFinishLocation(int pieceId);

	UFUNCTION(BlueprintPure)
	bool IsFinishPieceId(int pieceId);

	UFUNCTION(BlueprintPure)
	bool RequestToNextLocation(int pieceId,EPieceDirection direction,int& nextPieceId);

	TSubclassOf<class ABasePieceActor> GetPieceInstanceActorClass(UBasePiece* piece);
	
	UFUNCTION(BlueprintPure)
	TArray<UBasePiece*> GetFloorPieces(int floor);

	UFUNCTION(BlueprintPure)
	TArray<UBasePiece*> GetDirectionPiecesByFloorPieces(TArray<UBasePiece*> floorPieces,FPieceLocation curLocation,EPieceDirection direction);

	UFUNCTION(BlueprintPure)
    UBasePiece* GetNearPieceByDirectionPieces(int pieceId,FPieceDistance distance,EPieceDirection direction);

	UFUNCTION(BlueprintPure)
    UBasePiece* GetNearPieceByUpOrDown(int pieceId,FPieceDistance distance,EPieceDirection direction);
	
	UFUNCTION(BlueprintPure)
	TArray<UBasePiece*> GetOutDistancePieces(int pieceId,FPieceDistance distance,EPieceDirection direction);

	UFUNCTION(BlueprintPure)
	FPieceLocation GetNearLogicLocationByActorLocation(FVector location);

	UFUNCTION(BlueprintCallable)
	void UsePieceCardToLocation(FPieceLocation location,FName pieceCardName);
	
protected:
	UBasePiece* CreatePiece(UPieceBaseConfigData* pieceData);

	UPieceLocationInfo* GetLocationInfo(FPieceLocation location);

	class UPieceBaseAction* CreateAction(FPieceActionHandle& handle,class UPieceBaseActionConfigData* actionData,int pieceId = -1);
};
