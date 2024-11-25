// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Public/CookieLandMapActorGather.h"
#include "CookieLand/Map/Public/CookieLandPiece.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"

bool UCookieLandMapActorGather::GetIsPieceExistByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandMapCubeActorInfo** MapCubeInfoPtr = Locaiton2Pieces.Find(MapCubeLocation.ToVector());
	if (!MapCubeInfoPtr)
	{
		return false;
	}

	UCookieLandMapCubeActorInfo* MapCubeInfo = *MapCubeInfoPtr;
	UCookieLandPiece** PiecePtr = MapCubeInfo->OrientationPieces.Find(PieceOrientation);
	if (PiecePtr && *PiecePtr)
	{
		return true;
	}

	return false;
}

bool UCookieLandMapActorGather::AddPiece(const FCookieLandPieceBuildInfo PieceBuildInfo)
{
	if (GetIsPieceExistByLocation(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation))
	{
		return false;
	}

	UCookieLandPiece* Piece = CreatePieceToMapCubeInfo(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation);
	Pieces.Add(Piece);

	Piece->Init(PieceBuildInfo);

	return true;
}

bool UCookieLandMapActorGather::RemovePiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandPiece* Piece = GetPiece(MapCubeLocation, PieceOrientation);
	if (!Piece)
	{
		return false;
	}

	Piece->UnInit();

	Pieces.Remove(Piece);
	RemovePieceFromMapCubeInfo(MapCubeLocation, PieceOrientation);

	return true;
}


UCookieLandPiece* UCookieLandMapActorGather::GetPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandMapCubeActorInfo** MapCubeInfoPtr = Locaiton2Pieces.Find(MapCubeLocation.ToVector());
	if (!MapCubeInfoPtr)
	{
		return nullptr;
	}

	UCookieLandMapCubeActorInfo* MapCubeInfo = *MapCubeInfoPtr;
	UCookieLandPiece** PiecePtr = MapCubeInfo->OrientationPieces.Find(PieceOrientation);
	if (PiecePtr)
	{
		UCookieLandPiece* Piece = *PiecePtr;
		if (Piece)
		{
			return Piece;
		}
	}

	return nullptr;
}

UCookieLandPiece* UCookieLandMapActorGather::CreatePieceToMapCubeInfo(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandPiece* Piece = NewObject<UCookieLandPiece>();

	if (!Locaiton2Pieces.Contains(MapCubeLocation.ToVector()))
	{
		UCookieLandMapCubeActorInfo* MapCubeInfo = NewObject<UCookieLandMapCubeActorInfo>();
		MapCubeInfo->Location = MapCubeLocation;

		Locaiton2Pieces.Add(MapCubeLocation.ToVector(), MapCubeInfo);
	}

	UCookieLandMapCubeActorInfo* MapCubeInfo = Locaiton2Pieces[MapCubeLocation.ToVector()];
	MapCubeInfo->OrientationPieces.Add(PieceOrientation, Piece);

	return Piece;
}

void UCookieLandMapActorGather::RemovePieceFromMapCubeInfo(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandMapCubeActorInfo** MapCubeInfoPtr = Locaiton2Pieces.Find(MapCubeLocation.ToVector());
	if (!MapCubeInfoPtr)
	{
		return;
	}

	UCookieLandMapCubeActorInfo* MapCubeInfo = *MapCubeInfoPtr;
	MapCubeInfo->OrientationPieces.Remove(PieceOrientation);
}

TArray<UCookieLandPiece*> UCookieLandMapActorGather::GetAllPieces()
{
	return Pieces;
}