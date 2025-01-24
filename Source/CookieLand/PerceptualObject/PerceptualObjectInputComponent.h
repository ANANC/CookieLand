// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputMappingContext.h"
#include "CookieLand/Map/CookieLandMapTypes.h"
#include "PerceptualObjectInputComponent.generated.h"

class UEnhancedInputComponent;
class UCookieLandPerceptualObjectComponent;
class ACookieLandMapBuildActor;
class ABasePerceptualObjectController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOKIELAND_API UPerceptualObjectInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPerceptualObjectInputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

protected:
	UPROPERTY()
	TObjectPtr< UEnhancedInputComponent> EnhancedInputComponent;

	UPROPERTY()
	TObjectPtr< UCookieLandPerceptualObjectComponent> PerceptualObjectComponent;

public:
	void SetupInputComponent(UEnhancedInputComponent* InEnhancedInputComponent);

	void SetPawn(APawn* InPawn);
protected:
	UFUNCTION()
	void MoveActionTriggerCallback(const FInputActionValue& OutValue);

	bool GetNextPiece(ECookieLandPieceOrientation Orientation, FCookieLandPieceLocator& OutLocator);

	TObjectPtr<ACookieLandMapBuildActor> GetMainMapBuildActor();

	ABasePerceptualObjectController* GetController();

	ACharacter* GetCharacter();
};
