// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInstanceCubeActor.generated.h"

UCLASS()
class COOKIELAND_API ABaseInstanceCubeActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseInstanceCubeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	int CubeId;

	UPROPERTY(BlueprintReadOnly)
	class UBaseInstanceCube* Cube;
	
	UPROPERTY(BlueprintReadOnly)
	class UBasePieceLand* OwnLand;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void InitialArt();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateArt();
};
