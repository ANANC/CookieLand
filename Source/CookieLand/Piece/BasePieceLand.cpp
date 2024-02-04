﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePieceLand.h"

#include "BasePiece.h"
#include "BasePieceActor.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"

void UBasePieceLand::CreateLand(FName levelName,class ULandDataAsset* landDA)
{
	LevelName = levelName;
	LandDataAsset = landDA;

	BoundInfo = NewObject<UPieceLandBoundInfo>();
	
	for(int index = 0;index<LandDataAsset->Pieces.Num();++index)
	{
		TObjectPtr<UPieceBaseConfigData> pieceData = LandDataAsset->Pieces[index];

		CreatePiece(pieceData.Get());
	}
}

void UBasePieceLand::DestroyLand()
{
	for(int index = 0;index<Pieces.Num();++index)
	{
		Pieces[index]->UnInit();
	}

	Pieces.Reset();
	PieceMap.Reset();
}

UBasePiece* UBasePieceLand::CreatePiece(UPieceBaseConfigData* pieceData)
{
	UBasePiece* piece = NewObject<UBasePiece>(this);
	Pieces.Add(piece);
	PieceMap.Add(pieceData->Id,piece);
	
	piece->SetOwnLand(this);
	piece->SetId(pieceData->Id);
	piece->SetConfigData(pieceData);

	piece->Init();

	BoundInfo->AddPiece(piece);
	
	return piece;
}

bool UBasePieceLand::GetEnableOccupyLocation(FPieceLocation location,int& OccupyId)
{
	UPieceLocationInfo* locationInfo = GetLocationInfo(location);

	OccupyId = locationInfo->PieceId;

	if(locationInfo->IsOccupy || !locationInfo->EnableOccupy)
	{
		return false;
	}

	return true;
}

bool UBasePieceLand::RequestOccupyLocation(int Id,FPieceLocation location)
{
	int OccupyId;
	if(!GetEnableOccupyLocation(location,OccupyId))
	{
		return false;
	}
	
	UPieceLocationInfo* locationInfo = GetLocationInfo(location);
	if(locationInfo->EnableOccupy)
	{
		locationInfo->IsOccupy = true;
		locationInfo->PieceId = Id;

		return true;
	}

	return false;
}

bool UBasePieceLand::RequestUnOccupyLocation(int Id,FPieceLocation location)
{
	UPieceLocationInfo* locationInfo = GetLocationInfo(location);
	if(locationInfo->IsOccupy && locationInfo->PieceId == Id)
	{
		locationInfo->IsOccupy = false;
		locationInfo->PieceId = -1;

		return true;
	}

	return false;
}

FName UBasePieceLand::GetLevelName()
{
	return LevelName;
}

int UBasePieceLand::GetInitialPieceId()
{
	if(LandDataAsset)
	{
		return LandDataAsset->InitialPieceId;
	}
	return -1;
}

UBasePiece* UBasePieceLand::GetPieceById(int Id)
{
	UBasePiece** piecePtr = PieceMap.Find(Id);
	if(piecePtr)
	{
		return *piecePtr;
	}
	return nullptr;
}

FVector UBasePieceLand::GetActorLocationById(int Id)
{
	FVector actorLocation = FVector::ZeroVector;
	
	UBasePiece* piece = GetPieceById(Id);
	if(piece)
	{
		const UPieceInfo* curInfo = piece->GetCurInfo();
		if(curInfo->IsOccupy)
		{
			actorLocation = GetActorLocationByLocation(curInfo->Info->Location);
		}
	}

	return actorLocation;
}

FVector UBasePieceLand::GetActorLocationByLocation(FPieceLocation location)
{
	FVector actorLocation = LandDataAsset->CreateInitialLocation;
	actorLocation.X += location.X * LandDataAsset->ActorInterval.X;
	actorLocation.Y += location.Y * LandDataAsset->ActorInterval.Y;
	actorLocation.Z += location.Floor * LandDataAsset->ActorInterval.Z;

	return actorLocation;
}

UPieceLocationInfo* UBasePieceLand::GetLocationInfo(FPieceLocation location)
{
	for(int index = 0;index<OccupyStates.Num();++index)
	{
		if(OccupyStates[index]->Location == location)
		{
			return OccupyStates[index];
		}
	}

	UPieceLocationInfo* locationInfo = NewObject<UPieceLocationInfo>();
	locationInfo->Location = location;
	OccupyStates.Add(locationInfo);

	return locationInfo;
}

bool UBasePieceLand::GetPieceIdByLocation(FPieceLocation location,int& pieceId)
{
	for(int index = 0;index<OccupyStates.Num();++index)
	{
		if(OccupyStates[index]->Location == location)
		{
			if(OccupyStates[index]->IsOccupy)
			{
				pieceId = OccupyStates[index]->PieceId;
				return true;
			}
		}
	}

	return false;
}
	
bool UBasePieceLand::IsInFinishLocation(int pieceId)
{
	bool isFinishLocation = LandDataAsset?LandDataAsset->FinishPieceId == pieceId:false;
	return isFinishLocation;
}

bool UBasePieceLand::IsFinishPieceId(int pieceId)
{
	return LandDataAsset?LandDataAsset->FinishPieceId == pieceId:false;
}

bool UBasePieceLand::RequestToNextLocation(int pieceId,EPieceDirection direction,int& nextPieceId)
{
	UBasePiece* piece = GetPieceById(pieceId);
	if(!piece)
	{
		return false;	
	}

	if(!piece->GetEnableMove(direction))
	{
		return false;
	}

	FPieceDistance distance = piece->GetCurInfo()->Info->Distance;
	
	UBasePiece* nextPiece = nullptr;
	if(direction != EPieceDirection::Up && direction != EPieceDirection::Down)
	{
		nextPiece = GetNearPieceByDirectionPieces(pieceId,distance,direction);
	}
	else
	{
		nextPiece = GetNearPieceByUpOrDown(pieceId,distance,direction);
	}
	
	if(!nextPiece)
	{
		return false;
	}

	nextPieceId = nextPiece->GetId();
	return true;
}

TArray<UBasePiece*> UBasePieceLand::GetFloorPieces(int floor)
{
	UPieceLandFloorBoundInfo** floorBoundInfoPtr = BoundInfo->Floor2BoundInfos.Find(floor);
	if(floorBoundInfoPtr)
	{
		UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
		return floorBoundInfo->Pieces;
	}

	TArray<UBasePiece*> floorPieces;
	return  floorPieces;
}

TArray<UBasePiece*> UBasePieceLand::GetDirectionPiecesByFloorPieces(TArray<UBasePiece*> floorPieces,FPieceLocation curLocation,EPieceDirection direction)
{
	TArray<UBasePiece*> directionPieces;

	for(int index = 0;index<floorPieces.Num();++index)
	{
		UBasePiece* piece = floorPieces[index];
		FPieceLocation pieceLocation = piece->GetCurInfo()->Info->Location;

		if(direction == EPieceDirection::Left && (pieceLocation.Y == curLocation.Y && pieceLocation.X < curLocation.X))
		{
			directionPieces.Add(piece);
		}
		if(direction == EPieceDirection::Right && (pieceLocation.Y == curLocation.Y && pieceLocation.X > curLocation.X))
		{
			directionPieces.Add(piece);
		}
		if(direction == EPieceDirection::Forward && (pieceLocation.X == curLocation.X && pieceLocation.Y < curLocation.Y))
		{
			directionPieces.Add(piece);
		}
		if(direction == EPieceDirection::Backward && (pieceLocation.X == curLocation.X && pieceLocation.Y > curLocation.Y))
		{
			directionPieces.Add(piece);
		}
	}

	return directionPieces;
}

UBasePiece* UBasePieceLand::GetNearPieceByDirectionPieces(int pieceId,FPieceDistance distance,EPieceDirection direction)
{
	UBasePiece* piece = GetPieceById(pieceId);
	FPieceLocation curLocation = piece->GetCurInfo()->Info->Location;
	
	TArray<UBasePiece*> floorPieces = GetFloorPieces(curLocation.Floor);
	TArray<UBasePiece*> directionPieces = GetDirectionPiecesByFloorPieces(floorPieces,curLocation,direction);
	
	UBasePiece* minPiece = nullptr;
	FPieceLocation minLocation(0,0,0);
	
	for(int index = 0;index<directionPieces.Num();++index)
	{
		bool update = false;
		UBasePiece* directionPiece = directionPieces[index];
		FPieceLocation pieceLocation = directionPiece->GetCurInfo()->Info->Location;
		if(direction == EPieceDirection::Left)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.X-curLocation.X)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.X < pieceLocation.X);
		}
		if(direction == EPieceDirection::Right)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.X-curLocation.X)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.X > pieceLocation.X);
		}
		if(direction == EPieceDirection::Forward)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.Y-curLocation.Y)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.Y < pieceLocation.Y);
		}
		if(direction == EPieceDirection::Backward)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.Y-curLocation.Y)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.Y > pieceLocation.Y);
		}

		if(update)
		{
			minPiece = directionPiece;
			minLocation.Copy(pieceLocation);
		}
	}

	if(minPiece)
	{
		EPieceDirection oppositeDir = UCommonFunctionLibrary::ChangeGameDirectionByAngle(180,direction);
		if(!minPiece->GetEnableMove(EPieceDirection::Up) && !minPiece->GetEnableMove(EPieceDirection::Down)
			&& !IsFinishPieceId(minPiece->GetId())
			&& !minPiece->GetEnableMove(oppositeDir))
		{
			minPiece = nullptr;
		}
	}
	return minPiece;
}

UBasePiece* UBasePieceLand::GetNearPieceByUpOrDown(int pieceId,FPieceDistance distance,EPieceDirection direction)
{
	UBasePiece* piece = GetPieceById(pieceId);
	FPieceLocation curLocation = piece->GetCurInfo()->Info->Location;
	int intDirection = direction == EPieceDirection::Up?1:direction==EPieceDirection::Down?-1:0;

	int range;
	if(distance.IsUnLimit)
	{
		if(BoundInfo->HasValidFloors())
		{
			range = direction == EPieceDirection::Up?BoundInfo->MaxFloor:BoundInfo->MinFloor;
		}
		else
		{
			range = curLocation.Floor;
		}
	}
	else
	{
		range = curLocation.Floor + distance.LimitDistance * intDirection;
	}
	
	for(int index = 1, floor = curLocation.Floor;index<=abs(range-curLocation.Floor);++index)
	{
		floor += intDirection;
		FPieceLocation nextLocation(curLocation,floor);

		int nextPieceId;
		if(GetPieceIdByLocation(nextLocation,nextPieceId))
		{
			UBasePiece* nextPiece = GetPieceById(nextPieceId);
			return nextPiece;
		}
	}

	return nullptr;
}


TArray<UBasePiece*> UBasePieceLand::GetOutDistancePieces(int pieceId,FPieceDistance distance,EPieceDirection direction)
{
	TArray<UBasePiece*> outDistancePieces;
	if(distance.IsUnLimit)
	{
		return outDistancePieces;
	}
	
	UBasePiece* piece = GetPieceById(pieceId);
	FPieceLocation pieceLocation = piece->GetCurInfo()->Info->Location;

	int initialIndex = 0;
	int intDirection = (direction == EPieceDirection::Left || direction == EPieceDirection::Backward || direction == EPieceDirection::Down)?-1:1;
	int range = 0;
	
	UPieceLandFloorBoundInfo* floorBoundInfo = BoundInfo->GetFloorBoundInfo(pieceLocation.Floor);
	if(!floorBoundInfo)
	{
		return outDistancePieces;
	}
	
	switch (direction)
	{
	case EPieceDirection::Left:
	case EPieceDirection::Right:
		initialIndex = pieceLocation.X + distance.LimitDistance * intDirection;
		range = intDirection == 1? floorBoundInfo->MaxX:floorBoundInfo->MinX;
		break;
	case EPieceDirection::Forward:
	case EPieceDirection::Backward:
		initialIndex = pieceLocation.Y + distance.LimitDistance * intDirection;
		range = intDirection == 1? floorBoundInfo->MaxY:floorBoundInfo->MinY;
		break;
	case EPieceDirection::Up:
	case EPieceDirection::Down:
		initialIndex = piece->GetCurInfo()->Info->Location.Floor + distance.LimitDistance * intDirection;
		range = intDirection == 1? BoundInfo->MaxFloor:BoundInfo->MinFloor;
		break;
	}
	
	for(int index = 0,value = initialIndex;index<abs(range-initialIndex);++index)
	{
		FPieceLocation nextLocation(pieceLocation);
		value+=intDirection;
		
		switch (direction)
		{
		case EPieceDirection::Left:
		case EPieceDirection::Right:
			nextLocation.X = value;
			break;
		case EPieceDirection::Forward:
		case EPieceDirection::Backward:
			nextLocation.Y = value;
			break;
		case EPieceDirection::Up:
		case EPieceDirection::Down:
			nextLocation.Floor = value;
			break;
		}
		int nextPieceId;
		if(GetPieceIdByLocation(nextLocation,nextPieceId))
		{
			UBasePiece* nextPiece = GetPieceById(nextPieceId);
			outDistancePieces.Add(nextPiece);
		}
	}

	return outDistancePieces;
}

TSubclassOf<class ABasePieceActor> UBasePieceLand::GetPieceInstanceActorClass(UBasePiece* piece)
{
	TSubclassOf<class ABasePieceActor> actorClassType = nullptr;
	if(LandDataAsset)
	{
		actorClassType = LandDataAsset->DefaultActorClass;
	}
	
	if(!actorClassType)
	{
		actorClassType = piece->GetConfigData()->ActorClass;
	}

	if(!actorClassType)
	{
		actorClassType = ABasePieceActor::StaticClass();
	}

	return actorClassType;
}
