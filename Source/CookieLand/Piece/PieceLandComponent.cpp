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

			if(curLand->IsInFinishLocation(CurLocation))
			{
				StandByFinishLocation();
			}
		}
	}
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
	CurLocation = location;
	
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		FVector newLocation = curLand->GetActorLocationByOccupyLocation(CurLocation);
		MoveToNextLocation(newLocation);
	}
}