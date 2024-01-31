// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "CharacterLocomotionComponent.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/Piece/BasePieceLand.h"
#include "CookieLand/Piece/PieceLandComponent.h"
#include "CookieLand/Piece/PieceLandSystem.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	bClientUpdating = true;
	PieceLandComponent = CreateDefaultSubobject<UPieceLandComponent>(TEXT("PieceLandComponent"));
	LocomotionComponent = CreateDefaultSubobject<UCharacterLocomotionComponent>(TEXT("LocomotionComponent"));
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UPieceLandComponent* ABaseCharacter::GetPieceLandComponent()
{
	return PieceLandComponent;
}

UCharacterLocomotionComponent* ABaseCharacter::GetLocomotionComponent()
{
	return LocomotionComponent;
}