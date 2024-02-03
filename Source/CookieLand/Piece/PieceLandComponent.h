// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PieceTypes.h"
#include "PieceLandComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMoveToNextPieceEvent, FPieceLocation, oldLocation,FPieceLocation,newLocation);
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

protected:
	UPROPERTY(BlueprintReadOnly)
	int CurPieceId{-1};

	UPROPERTY(BlueprintAssignable)
	FMoveToNextPieceEvent MoveToNextPieceEvent;
	
	UPROPERTY(BlueprintAssignable)
	FStandByFinishPieceEvent StandByFinishPieceEvent;
	
	UPROPERTY(BlueprintReadOnly)
	class ABaseCharacter* Character;
	
public:
	UFUNCTION(BlueprintCallable)
	void ResetLocationToInitialPiece();
	
	UFUNCTION(BlueprintCallable)
	void MoveToNextPiece(EPieceDirection direction);

	UFUNCTION(BlueprintPure)
	FPieceLocation GetCurLocation();
	
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
	
};
