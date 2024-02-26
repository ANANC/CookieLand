// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PieceTypes.h"
#include "GameFramework/Actor.h"
#include "BasePieceActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPieceActorInAnimationStateChangeEvent);

UCLASS()
class COOKIELAND_API ABasePieceActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePieceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

public:
	UPROPERTY(BlueprintAssignable)
	FPieceActorInAnimationStateChangeEvent PieceActorStartInAnimationEvent;
	
	UPROPERTY(BlueprintAssignable)
	FPieceActorInAnimationStateChangeEvent PieceActorFinishInAnimationEvent;
	
protected:
	
	UPROPERTY(BlueprintReadOnly)
	class UBasePieceLand* OwnLand;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	class UBasePiece* Piece;

	UPROPERTY()
	bool IsInAnimation{false};
	
public:

	void SetOwnLand(class UBasePieceLand* land);

	void SetPiece(class UBasePiece* piece);

	void Init();

public:
	UFUNCTION(BlueprintPure)
	FPieceLocation GetMyLocation();
	
	UFUNCTION(BlueprintPure)
	bool GetIsPlayerStandOnMyself();
	
	UFUNCTION(BlueprintPure)
	bool GetIsPlayerStandOnThisFloor();
	
	UFUNCTION(BlueprintPure)
	bool GetIsFinishPieceByMyself();
	
	UFUNCTION(BlueprintPure)
	FPieceObserveStateData GetPieceObserveState();
	
	UFUNCTION(BlueprintPure)
	bool GetIsInAnimation();

	UFUNCTION(BlueprintCallable)
	void SetIsInAnimation(bool isAnimation);
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void InitArt();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StandToInitialPieceEventCallback(FPieceLocation location);
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToNextPieceEventCallback(FPieceLocation oldLocation, FPieceLocation newLocation);
	
    UFUNCTION(BlueprintImplementableEvent)
    void TriggerActionArt_RemindDropOut();
	
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerActionArt_DropOut();
	
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerActionArt_Rotation();     
};
