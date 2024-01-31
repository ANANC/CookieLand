// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAnimTypes.h"

#include "CharacterLocomotionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COOKIELAND_API UCharacterLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterLocomotionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(VisibleAnywhere)
	ELSGait LSGait{ELSGait::Walking};
	
	UPROPERTY(VisibleAnywhere)
	ELSMovementMode LSMovementMode{ELSMovementMode::Grounded};
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintPure)
	ELSGait GetLSGait();

	UFUNCTION(BlueprintPure)
	ELSMovementMode GetLSMovementMode();
};
