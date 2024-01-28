// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePieceLand.h"

#include "BasePiece.h"
#include "BasePieceActor.h"

void UBasePieceLand::CreateLand(FName levelName,class ULandDataAsset* landDA)
{
	LevelName = levelName;
	LandDataAsset = landDA;

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
			actorLocation = GetActorLocationByOccupyLocation(curInfo->Info->Location);
		}
	}

	return actorLocation;
}

FVector UBasePieceLand::GetActorLocationByOccupyLocation(FPieceLocation location)
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
	
bool UBasePieceLand::IsInFinishLocation(FPieceLocation location)
{
	int pieceId;
	if(GetPieceIdByLocation(location,pieceId))
	{
		bool isFinishLocation = LandDataAsset?LandDataAsset->FinishPieceId == pieceId:false;
		return isFinishLocation;
	}

	return false;
}

bool UBasePieceLand::IsFinishPieceId(int pieceId)
{
	return LandDataAsset?LandDataAsset->FinishPieceId == pieceId:false;
}

bool UBasePieceLand::RequestToNextLocation(FPieceLocation curLocation,EPieceDirection direction,FPieceLocation& newLocation)
{
	int pieceId;
	if(!GetPieceIdByLocation(curLocation,pieceId))
	{
		return false;
	}
	
	UBasePiece* piece = GetPieceById(pieceId);
	if(!piece)
	{
		return false;	
	}

	if(!piece->GetEnableMove(direction))
	{
		return false;
	}

	TArray<UBasePiece*> floorPieces = GetFloorPieces(curLocation.Floor);
	TArray<UBasePiece*> directionPieces = GetDirectionPiecesByFloorPieces(floorPieces,curLocation,direction);
	UBasePiece* nextPiece = GetNearPieceByDirectionPieces(directionPieces,curLocation,piece->GetCurInfo()->Info->Distance,direction);

	if(!nextPiece)
	{
		return false;
	}

	newLocation = nextPiece->GetCurInfo()->Info->Location;
	return true;
}

TArray<UBasePiece*> UBasePieceLand::GetFloorPieces(int floor)
{
	TArray<UBasePiece*> floorPieces;
	
	for(int index = 0;index<Pieces.Num();++index)
	{
		if(Pieces[index]->GetCurInfo()->Info->Location.Floor == floor)
		{
			floorPieces.Add(Pieces[index]);
		}
	}

	return  floorPieces;
}

TArray<UBasePiece*> UBasePieceLand::GetDirectionPiecesByFloorPieces(TArray<UBasePiece*> floorPieces,FPieceLocation curLocation,EPieceDirection direction)
{
	TArray<UBasePiece*> directionPieces;

	for(int index = 0;index<floorPieces.Num();++index)
	{
		FPieceLocation pieceLocation = Pieces[index]->GetCurInfo()->Info->Location;

		if(direction == EPieceDirection::Left && (pieceLocation.Y == curLocation.Y && pieceLocation.X < curLocation.X))
		{
			directionPieces.Add(Pieces[index]);
		}
		if(direction == EPieceDirection::Right && (pieceLocation.Y == curLocation.Y && pieceLocation.X > curLocation.X))
		{
			directionPieces.Add(Pieces[index]);
		}
		if(direction == EPieceDirection::Forward && (pieceLocation.X == curLocation.X && pieceLocation.Y > curLocation.Y))
		{
			directionPieces.Add(Pieces[index]);
		}
		if(direction == EPieceDirection::Backward && (pieceLocation.X == curLocation.X && pieceLocation.Y < curLocation.Y))
		{
			directionPieces.Add(Pieces[index]);
		}
	}

	return directionPieces;
}

UBasePiece* UBasePieceLand::GetNearPieceByDirectionPieces(TArray<UBasePiece*> directionPieces,FPieceLocation curLocation,FPieceDistance Distance,EPieceDirection direction)
{
	UBasePiece* minPiece = nullptr;
	FPieceLocation minLocation;
	
	for(int index = 0;index<directionPieces.Num();++index)
	{
		bool update = false;
		FPieceLocation pieceLocation = Pieces[index]->GetCurInfo()->Info->Location;
		if(direction == EPieceDirection::Left)
		{
			if(!Distance.LimitDistance && (abs(pieceLocation.X-curLocation.X)>Distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.X < pieceLocation.X);
		}
		if(direction == EPieceDirection::Right)
		{
			if(!Distance.LimitDistance && (abs(pieceLocation.X-curLocation.X)>Distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.X > pieceLocation.X);
		}
		if(direction == EPieceDirection::Forward)
		{
			if(!Distance.LimitDistance && (abs(pieceLocation.Y-curLocation.Y)>Distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.Y > pieceLocation.Y);
		}
		if(direction == EPieceDirection::Backward)
		{
			if(!Distance.LimitDistance && (abs(pieceLocation.Y-curLocation.Y)>Distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.Y < pieceLocation.Y);
		}

		if(update)
		{
			minPiece = Pieces[index];
			minLocation = pieceLocation;
		}
	}

	return minPiece;
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