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
	UPieceLandFloorBoundInfo** floorBoundInfoPtr = Floor2BoundInfos.Find(location.Floor);
	if(floorBoundInfoPtr)
	{
		UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
		floorBoundInfo->Pieces.Add(piece);
	}
	else
	{
		UPieceLandFloorBoundInfo* floorBoundInfo= NewObject<UPieceLandFloorBoundInfo>();
		Floor2BoundInfos.Add(location.Floor,floorBoundInfo);
		ValidFloors.Add(location.Floor);
		
		floorBoundInfo->Floor = location.Floor;
		floorBoundInfo->Pieces.Add(piece);
	}
}

void UPieceLandBoundInfo::RemovePiece(class UBasePiece* piece)
{
	if(!piece)
	{
		return;
	}
	FPieceLocation location = piece->GetCurInfo()->Info->Location;
	UPieceLandFloorBoundInfo** floorBoundInfoPtr = Floor2BoundInfos.Find(location.Floor);
	if(floorBoundInfoPtr)
	{
		UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
		floorBoundInfo->Pieces.Remove(piece);

		if(floorBoundInfo->Pieces.Num() == 0)
		{
			ValidFloors.Remove(location.Floor);
		}
	}
}