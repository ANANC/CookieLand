// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "CharacterLocomotionComponent.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/MoveTo/MoveToComponent.h"
#include "CookieLand/Piece/BasePieceLand.h"
#include "CookieLand/Piece/PieceCardComponent.h"
#include "CookieLand/Piece/PieceLandComponent.h"
#include "CookieLand/Piece/PieceLandSystem.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	bClientUpdating = true;
	PieceLandComponent = CreateDefaultSubobject<UPieceLandComponent>(TEXT("PieceLandComponent"));
	PieceCardComponent = CreateDefaultSubobject<UPieceCardComponent>(TEXT("PieceCardComponent"));
	LocomotionComponent = CreateDefaultSubobject<UCharacterLocomotionComponent>(TEXT("LocomotionComponent"));
	MoveToComponent = CreateDefaultSubobject<UMoveToComponent>(TEXT("MoveToComponent"));
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

UPieceCardComponent* ABaseCharacter::GetPieceCardComponent()
{
	return PieceCardComponent;
}

UCharacterLocomotionComponent* ABaseCharacter::GetLocomotionComponent()
{
	return LocomotionComponent;
}

UCharacterMovementComponent* ABaseCharacter::GetCharacterMovementComponent()
{
	return Cast<UCharacterMovementComponent>(GetMovementComponent());
}

UMoveToComponent* ABaseCharacter::GetMoveToComponent()
{
	return MoveToComponent;
}