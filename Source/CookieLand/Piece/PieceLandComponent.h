// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PieceTypes.h"
#include "PieceLandComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMoveToNextPieceEvent, FPieceLocation, oldLocation, FPieceLocation, newLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStandByFinishPieceEvent, FPieceLocation, location);

UCLASS(Blueprintable,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COOKIELAND_API UPieceLandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPieceLandComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:

	UPROPERTY(BlueprintAssignable)
	FMoveToNextPieceEvent MoveToNextPieceEvent;
	
	UPROPERTY(BlueprintAssignable)
	FStandByFinishPieceEvent StandByFinishPieceEvent;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	int LastPieceId{-1};

	UPROPERTY(BlueprintReadOnly)
	int CurPieceId{-1};
	
	UPROPERTY(BlueprintReadOnly)
	class ABaseCharacter* Character;

	UPROPERTY(BlueprintReadOnly)
	class UBasePieceLand* CurLand;
	
public:
	
	UFUNCTION(BlueprintCallable)
	void MoveToNextPiece(EPieceDirection direction);

	UFUNCTION(BlueprintPure)
	FPieceLocation GetCurLocation();
	
	UFUNCTION(BlueprintPure)
	FPieceLocation GetLastLocation();

protected:
	UFUNCTION()
	void CreatePieceLandEventCallback(FName levelName,int initialPieceId);
	
	void SetInitialLocation(int pieceId);
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToInitialLocation(int pieceId,FVector location);
	
	void SetCurLocation(int pieceId,EPieceDirection direction);
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToNextLocation(int pieceId,FVector location,EPieceDirection moveDirection);

	UFUNCTION(BlueprintImplementableEvent)
	void StandByFinishLocation();

	UFUNCTION()
	void LandLocationUnOccupyStateChangeEventCallback(int Id, FPieceLocation location);

	void FailedControl();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_FailedControl();
	
	UFUNCTION()
	void LandDestroyBeforeEventCallback();
};
