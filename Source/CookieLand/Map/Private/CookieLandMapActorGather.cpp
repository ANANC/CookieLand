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

	if (PieceBuildInfo.bAutoCreateActorInstance)
	{
		TryCreatePieceActorToPiece(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation);
	}

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

void UCookieLandMapActorGather::ChangePieceActorType(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation, TSubclassOf< ACookieLandPieceActor> InPieceActorType)
{
	UCookieLandPiece* Piece = GetPiece(MapCubeLocation, PieceOrientation);
	if (!Piece)
	{
		return;
	}

	if (ACookieLandPieceActor* ActorInstance = Piece->GetPieceAction()) 
	{
		if (ActorInstance && ActorInstance->GetClass() != InPieceActorType)
		{
			Piece->SetPieceActor(nullptr);
			ActorInstance->Destroy();
		}
	}

	Piece->SetPieceActorType(InPieceActorType);

	if (Piece->GetBuildInfo().bAutoCreateActorInstance)
	{
		TryCreatePieceActorToPiece(MapCubeLocation, PieceOrientation);
	}

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

ACookieLandPieceActor* UCookieLandMapActorGather::CreatePieceAction(const FCookieLandPieceBuildInfo PieceBuildInfo)
{
	ACookieLandPieceActor* ActorInstance = UCookieLandMapBuildLibrary::CreatePieceActorInstanceByBuildInfo(this, MapBuildInfo, PieceBuildInfo);
	return ActorInstance;
}

bool UCookieLandMapActorGather::TryCreatePieceActorToPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandPiece* Piece = GetPiece(MapCubeLocation, PieceOrientation);
	if (!Piece)
	{
		return false;
	}

	if (Piece->GetPieceAction())
	{
		return true;
	}

	ACookieLandPieceActor* ActorInstance = CreatePieceAction(Piece->GetBuildInfo());
	if (ActorInstance)
	{
		ActorInstance->Init(Piece);
		Piece->SetPieceActor(ActorInstance);

		return true;
	}

	return false;
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