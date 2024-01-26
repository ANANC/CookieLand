// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePieceActor.generated.h"

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

protected:
	
	UPROPERTY(BlueprintReadOnly)
	class UBasePieceLand* OwnLand;
	
	UPROPERTY(BlueprintReadWrite)
	class UBasePiece* Piece;
	
public:

	void SetOwnLand(class UBasePieceLand* land);

	void SetPiece(class UBasePiece* piece);

	void Init();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void InitArt();
};
