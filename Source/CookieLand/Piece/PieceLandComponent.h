// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PieceTypes.h"
#include "PieceLandComponent.generated.h"


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
	FPieceLocation CurLocation;
	
public:
	
	UFUNCTION(BlueprintCallable)
	void MoveToNextPiece(EPieceDirection direction);

	UFUNCTION(BlueprintPure)
	FPieceLocation GetCurLocation();
protected:
	UFUNCTION()
	void CreatePieceLandEventCallback(FName levelName,int initialPieceId);
	
	void SetInitialLocation(FPieceLocation location);
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToInitialLocation(FVector location);
	
	void SetCurLocation(FPieceLocation location);
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToNextLocation(FVector location);

	UFUNCTION(BlueprintImplementableEvent)
	void StandByFinishLocation();
	
};
