// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePerceptualObjectController.generated.h"

class UPerceptualObjectInputComponent;

UCLASS()
class ABasePerceptualObjectController : public APlayerController
{
	GENERATED_BODY()

	ABasePerceptualObjectController();

protected:
	UPROPERTY()
	UPerceptualObjectInputComponent* PerceptualObjectInputComponent;

public:
	virtual void SetPawn(APawn* InPawn) override;

protected:
	virtual void SetupInputComponent() override;

public:

	virtual void UpdateRotation(float DeltaTime) override;

	// Converts a rotation from world space to gravity relative space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

private:
	FVector LastFrameGravity = FVector::ZeroVector;
	
};
