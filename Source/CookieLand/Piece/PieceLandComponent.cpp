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
		
		CurLand = PieceLandSystem->GetCurLand();
		if(CurLand)
		{
			CreatePieceLandEventCallback(CurLand->GetLevelName(),CurLand->GetInitialPieceId());
		}
	}
}

void UPieceLandComponent::MoveToNextPiece(EPieceDirection direction)
{
	if(CurLand)
	{
		int nextPieceId;
		if(CurLand->RequestToNextLocation(CurPieceId,direction,nextPieceId))
		{
			SetCurLocation(nextPieceId,direction);
		}
	}
}

FPieceLocation UPieceLandComponent::GetCurLocation()
{
	if(CurLand)
	{
		UBasePiece* piece = CurLand->GetPieceById(CurPieceId);
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
	if(CurLand)
	{
		UBasePiece* piece = CurLand->GetPieceById(LastPieceId);
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
	CurLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(CurLand)
	{
		UBasePiece* piece = CurLand->GetPieceById(initialPieceId);
		if(piece)
		{
			SetInitialLocation(piece->GetId());
		}

		CurLand->LandLocationUnOccupyStateChangeEvent.AddDynamic(this,&UPieceLandComponent::LandLocationUnOccupyStateChangeEventCallback);
		CurLand->LandDestroyBeforeEvent.AddDynamic(this,&UPieceLandComponent::LandDestroyBeforeEventCallback);
	}
}

void UPieceLandComponent::LandDestroyBeforeEventCallback()
{
	if(CurLand)
	{
		CurLand->LandLocationUnOccupyStateChangeEvent.RemoveAll(this);
		CurLand->LandDestroyBeforeEvent.RemoveAll(this);
	}

	CurLand = nullptr;
}

void UPieceLandComponent::SetInitialLocation(int pieceId)
{
	LastPieceId = -1;
	CurPieceId = pieceId;

	if(CurLand)
	{
		FVector newLocation = CurLand->GetActorLocationById(CurPieceId);
		MoveToInitialLocation(CurPieceId,newLocation);
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
	
	if(CurLand)
	{
		FVector newLogicLocation = CurLand->GetActorLocationById(CurPieceId);
		MoveToNextLocation(CurPieceId,newLogicLocation,direction);
		
		MoveToNextPieceEvent.Broadcast(oldLocation,newLocation);
		
		if(CurLand->IsInFinishLocation(CurPieceId))
		{
			StandByFinishLocation();

			StandByFinishPieceEvent.Broadcast(newLocation);
		}
	}
}

void UPieceLandComponent::LandLocationUnOccupyStateChangeEventCallback(int Id, FPieceLocation location)
{
	if(!CurLand)
	{
		return;
	}
	
	if(Id != CurPieceId)
	{
		return;
	}
	
	FailedControl();
}

void UPieceLandComponent::FailedControl()
{
	BP_FailedControl();
}