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

	UpdateBound();
}

void UPieceLandBoundInfo::RemovePiece(class UBasePiece* piece)
{
	if(!piece)
	{
		return;
	}
	
	FPieceLocation location = piece->GetCurInfo()->Info->Location;
	int floor = location.Floor;

	UPieceLandFloorBoundInfo* floorBoundInfo = GetFloorBoundInfo(floor);
	if(floorBoundInfo)
	{
		floorBoundInfo->Pieces.Remove(piece);

		if(floorBoundInfo->Pieces.Num() == 0)
		{
			ValidFloors.Remove(floor);
		}
	}

	UpdateBound();
}

void UPieceLandBoundInfo::UpdateBound()
{
	MaxFloor = MinFloor = 0;
	
	for(int index = 0;index<ValidFloors.Num();++index)
	{
		int floor = ValidFloors[index];
		if(floor > MaxFloor)
		{
			MaxFloor = floor;
		}
		if(floor < MinFloor)
		{
			MinFloor = floor;
		}

		UPieceLandFloorBoundInfo** floorBoundInfoPtr = Floor2BoundInfos.Find(floor);
		if(floorBoundInfoPtr)
		{
			UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
			
			floorBoundInfo->MaxX = floorBoundInfo->MinX = floorBoundInfo->MaxY = floorBoundInfo->MinY = 0;
			for(int j = 0;j<floorBoundInfo->Pieces.Num();++j)
			{
				FPieceLocation location = floorBoundInfo->Pieces[j]->GetCurInfo()->Info->Location;
				if(j==0 || location.X<floorBoundInfo->MinX)
				{
					floorBoundInfo->MinX = location.X;
				}
				if(j==0 || location.X>floorBoundInfo->MaxX)
				{
					floorBoundInfo->MaxX = location.X;
				}
				if(j==0 || location.Y<floorBoundInfo->MinY)
				{
					floorBoundInfo->MinY = location.Y;
				}
				if(j==0 || location.Y>floorBoundInfo->MaxY)
				{
					floorBoundInfo->MaxY = location.Y;
				}
			}
		}
	}
}

bool UPieceLandBoundInfo::HasValidFloors()
{
	return ValidFloors.Num()>0;
}

bool UPieceLandBoundInfo::HasValidPieceInFloor(int floor)
{
	UPieceLandFloorBoundInfo* floorBoundInfo = GetFloorBoundInfo(floor);
	if(floorBoundInfo)
	{
		return floorBoundInfo->Pieces.Num()>0;
	}
	return false;
}

UPieceLandFloorBoundInfo* UPieceLandBoundInfo::GetFloorBoundInfo(int floor)
{
	UPieceLandFloorBoundInfo** floorBoundInfoPtr = Floor2BoundInfos.Find(floor);
	if(floorBoundInfoPtr)
	{
		UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
		return floorBoundInfo;
	}
	return nullptr;
}