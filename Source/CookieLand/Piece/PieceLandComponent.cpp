// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceLandComponent.h"

#include "BasePiece.h"
#include "BasePieceLand.h"
#include "PieceLandSystem.h"
#include "CookieLand/Character/BaseCharacter.h"
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

	Character = Cast<ABaseCharacter>(GetOwner());
	
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

void UPieceLandComponent::ResetLocationToInitialPiece()
{
	UBasePieceLand* land = UCommonFunctionLibrary::GetCurPieceLand();
	if(land)
	{
		CreatePieceLandEventCallback(land->GetLevelName(),land->GetInitialPieceId());
		
		UCommonFunctionLibrary::GetPieceLandSystem()->PieceLandHideAllTipEvent.Broadcast(false,false);
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
			SetCurLocation(nextPieceId,direction);
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

FPieceLocation UPieceLandComponent::GetLastLocation()
{
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		UBasePiece* piece = curLand->GetPieceById(LastPieceId);
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
	//FPieceLocation oldLocation = GetCurLocation();

	LastPieceId = -1;
	CurPieceId = pieceId;

	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		FVector newLocation = curLand->GetActorLocationById(CurPieceId);
		MoveToInitialLocation(CurPieceId,newLocation);

		//MoveToNextPieceEvent.Broadcast(oldLocation,newLocation);
	}
}

void UPieceLandComponent::SetCurLocation(int pieceId,EPieceDirection direction)
{
	if(CurPieceId == pieceId)
	{
		return;
	}
	
	FPieceLocation oldLocation = GetCurLocation();

	LastPieceId = CurPieceId;
	CurPieceId = pieceId;

	FPieceLocation newLocation = GetCurLocation();
	
	UBasePieceLand* curLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(curLand)
	{
		FVector newLogicLocation = curLand->GetActorLocationById(CurPieceId);
		MoveToNextLocation(CurPieceId,newLogicLocation,direction);
		
		MoveToNextPieceEvent.Broadcast(oldLocation,newLocation);
	}
	
	if(curLand->IsInFinishLocation(CurPieceId))
	{
		StandByFinishLocation();

		StandByFinishPieceEvent.Broadcast(newLocation);
	}
}