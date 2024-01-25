// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "BaseCharacter.generated.h"

class UPieceLandComponent;

UCLASS()
class COOKIELAND_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UPieceLandComponent* PieceLandComponent;

public:
	
	UPieceLandComponent* GetPieceLandComponent();
};
