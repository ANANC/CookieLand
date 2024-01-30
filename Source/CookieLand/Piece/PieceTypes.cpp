// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceTypes.h"

#include "BasePiece.h"

void UPieceLandBoundInfo::AddPiece(class UBasePiece* piece)
{
	if(!piece)
	{
		return;
	}

	FPieceLocation location = piece->GetCurInfo()->Info->Location;
	int floor = location.Floor;
	
	UPieceLandFloorBoundInfo** floorBoundInfoPtr = Floor2BoundInfos.Find(location.Floor);
	if(floorBoundInfoPtr)
	{
		UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
		floorBoundInfo->Pieces.Add(piece);
	}
	else
	{
		UPieceLandFloorBoundInfo* floorBoundInfo= NewObject<UPieceLandFloorBoundInfo>();
		Floor2BoundInfos.Add(floor,floorBoundInfo);
		ValidFloors.Add(floor);
		
		floorBoundInfo->Floor = floor;
		floorBoundInfo->Pieces.Add(piece);
	}

	UpdateFloorMaxAndMin();
}

void UPieceLandBoundInfo::RemovePiece(class UBasePiece* piece)
{
	if(!piece)
	{
		return;
	}
	
	FPieceLocation location = piece->GetCurInfo()->Info->Location;
	int floor = location.Floor;
	
	UPieceLandFloorBoundInfo** floorBoundInfoPtr = Floor2BoundInfos.Find(location.Floor);
	if(floorBoundInfoPtr)
	{
		UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
		floorBoundInfo->Pieces.Remove(piece);

		if(floorBoundInfo->Pieces.Num() == 0)
		{
			ValidFloors.Remove(floor);
		}
	}

	UpdateFloorMaxAndMin();
}

void UPieceLandBoundInfo::UpdateFloorMaxAndMin()
{
	MinFloor = 0;
	MaxFloor = 0;
	for(int index = 0;index<ValidFloors.Num();++index)
	{
		if(ValidFloors[index] > MaxFloor)
		{
			MaxFloor = ValidFloors[index];
		}
		if(ValidFloors[index] < MinFloor)
		{
			MinFloor = ValidFloors[index];
		}
	}
}

bool UPieceLandBoundInfo::HasValidFloors()
{
	return ValidFloors.Num()>0;
}