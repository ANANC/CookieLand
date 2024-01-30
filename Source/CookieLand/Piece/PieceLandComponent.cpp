// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceLandComponent.h"

#include "BasePiece.h"
#include "BasePieceLand.h"
#include "PieceLandSystem.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"


// Sets default values for this component's properties
UPieceLandComponent::UPieceLandComponent()
{
}

// Called when the game starts
void UPieceLandComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	UPieceLandSystem* PieceLandSystem = UCommonFunctionLibrary::GetPieceLandSystem();
	if(PieceLandSystem)
	{
		PieceLandSystem->CreatePieceLandEvent.AddUObject(this,&UPieceLandComponent::CreatePieceLandEventCallback);
		
		UBasePieceLand* land = PieceLandSystem->GetCurLand();
		if(land)
		{
			CreatePieceLandEventCallback(land->GetLevelName(),land->GetInitialPieceId());
		}
	}
}

void UPieceLandComponent::MoveToNextPiece(EPieceDirection direction)
{
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		int nextPieceId;
		if(curLand->RequestToNextLocation(CurPieceId,direction,nextPieceId))
		{
			SetCurLocation(nextPieceId);
		}
	}
}

FPieceLocation UPieceLandComponent::GetCurLocation()
{
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		UBasePiece* piece = curLand->GetPieceById(CurPieceId);
		if(piece)
		{
			return piece->GetCurInfo()->Info->Location;
		}
	}
	FPieceLocation location(false);
	return location;
}

void UPieceLandComponent::CreatePieceLandEventCallback(FName levelName,int initialPieceId)
{
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		UBasePiece* piece = curLand->GetPieceById(initialPieceId);
		if(piece)
		{
			SetInitialLocation(piece->GetId());
		}
	}
}

void UPieceLandComponent::SetInitialLocation(int pieceId)
{
	CurPieceId = pieceId;

	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		FVector newLocation = curLand->GetActorLocationById(CurPieceId);
		MoveToInitialLocation(newLocation);
	}
}

void UPieceLandComponent::SetCurLocation(int pieceId)
{
	if(CurPieceId == pieceId)
	{
		return;
	}
	
	FPieceLocation oldLocation = GetCurLocation();
	
	CurPieceId = pieceId;

	FPieceLocation newLocation = GetCurLocation();
	
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		FVector newLogicLocation = curLand->GetActorLocationById(CurPieceId);
		MoveToNextLocation(newLogicLocation);
		
		MoveToNextPieceEvent.Broadcast(oldLocation,newLocation);
	}
	
	if(curLand->IsInFinishLocation(CurPieceId))
	{
		StandByFinishLocation();

		StandByFinishPieceEvent.Broadcast(newLocation);
	}
}