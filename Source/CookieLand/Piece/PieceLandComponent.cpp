// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceLandComponent.h"

#include "BasePiece.h"
#include "BasePieceLand.h"
#include "PieceLandSystem.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"


// Sets default values for this component's properties
UPieceLandComponent::UPieceLandComponent()
{
	CurLocation.IsValid = false;
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
		FPieceLocation newLocation;
		if(curLand->RequestToNextLocation(CurLocation,direction,newLocation))
		{
			SetCurLocation(newLocation);
		}
	}
}

FPieceLocation UPieceLandComponent::GetCurLocation()
{
	return CurLocation;
}

void UPieceLandComponent::CreatePieceLandEventCallback(FName levelName,int initialPieceId)
{
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		UBasePiece* piece = curLand->GetPieceById(initialPieceId);
		if(piece)
		{
			SetCurLocation(piece->GetCurInfo()->Info->Location);
		}
	}
}

void UPieceLandComponent::SetInitialLocation(FPieceLocation location)
{
	CurLocation = location;

	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		FVector newLocation = curLand->GetActorLocationByOccupyLocation(CurLocation);
		MoveToInitialLocation(newLocation);
	}
}

void UPieceLandComponent::SetCurLocation(FPieceLocation location)
{
	if(CurLocation == location)
	{
		return;
	}
	
	FPieceLocation curLocation = CurLocation;
	
	CurLocation = location;
	
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		FVector newLocation = curLand->GetActorLocationByOccupyLocation(CurLocation);
		MoveToNextLocation(newLocation);
		
		MoveToNextPieceEvent.Broadcast(curLocation,location);
	}
	
	if(curLand->IsInFinishLocation(CurLocation))
	{
		StandByFinishLocation();

		StandByFinishPieceEvent.Broadcast(location);
	}
}