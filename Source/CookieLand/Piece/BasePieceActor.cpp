// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePieceActor.h"


// Sets default values
ABasePieceActor::ABasePieceActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABasePieceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABasePieceActor::SetOwnLand(class UBasePieceLand* land)
{
	OwnLand = land;
}

void ABasePieceActor::SetPiece(class UBasePiece* piece)
{
	Piece = piece;	
}

void ABasePieceActor::Init()
{
	InitArt();
}