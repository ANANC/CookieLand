// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceTypes.h"

#include "BasePiece.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"

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

bool FInstanceCubeVolumeUnit::GetEnableBind(FInstanceCubeVolumeUnit other)
{
	if(Location.Floor != other.Location.Floor || Volume.Floor != other.Volume.Floor)
	{
		return false;
	}

	TArray<FPieceLocation> myLocations = GetLocations(Location.Floor);
	TArray<FPieceLocation> otherLocations = GetLocations(other.Location.Floor);

	FPieceDistance distance(false,1);
	
	for(int i = 0;i<myLocations.Num();++i)
	{
		FPieceLocation location1 = myLocations[i];
		for(int j =0;j<otherLocations.Num();++j)
		{
			FPieceLocation location2 = otherLocations[j];
			if(UCommonFunctionLibrary::IsLocationInSideByDistance(location1,location2,distance))
			{
				return true;
			}
		}
	}

	return false;
}

TArray<FPieceLocation> FInstanceCubeVolumeUnit::GetLocations(int floor)
{
	TArray<FPieceLocation> myLocations;
	for(int x = 0;x<Volume.X;++x)
	{
		for(int y = 0;y<Volume.Y;++y)
		{
			FPieceLocation location = Location;
			location.X += x;
			location.Y += y;
			location.Floor = floor;
			myLocations.Add(location);
		}
	}

	return myLocations;
}

bool FInstanceCubeVolumeUnit::TryMerge(FInstanceCubeVolumeUnit other)
{
	if((Volume.X == 1 && other.Volume.X == 1) && (Location.X == other.Location.X))
	{
		FPieceLocation newLocation = Location;
		newLocation.Y = FMath::Min(Location.Y,other.Location.Y);

		FPieceLocation newVolume = Volume;
		newVolume.Y += other.Volume.Y;

		Location = newLocation;
		Volume = newVolume;
		return true;
	}
	else if((Volume.Y == 1 && other.Volume.Y == 1) && (Location.Y == other.Location.Y))
	{
		FPieceLocation newLocation = Location;
		newLocation.X = FMath::Min(Location.X,other.Location.X);

		FPieceLocation newVolume = Volume;
		newVolume.X += other.Volume.X;

		Location = newLocation;
		Volume = newVolume;
		return true;
	}

	return false;
}

bool FInstanceCubeVolumeUnit::GetIsInSide(FPieceLocation location)
{
	if(location.Floor >= Location.Floor && location.Floor <= Location.Floor + Volume.Floor)
	{
		if(location.X >= Location.X && location.X <= Location.X + (Volume.X-1))
		{
			if(location.Y >= Location.Y && location.Y <= Location.Y + (Volume.Y-1))
			{
				return true;
			}
		}
	}
	return false;
}

bool FInstanceCubeVolumeUnit::GetIsInEdge(FPieceLocation location)
{
	if(location.Floor == Location.Floor || location.Floor == Location.Floor + Volume.Floor)
	{
		if(location.X == Location.X || location.X == Location.X + (Volume.X-1))
		{
			if(location.Y == Location.Y || location.Y == Location.Y + (Volume.Y-1))
			{
				return true;
			}
		}
	}

	return false;
}

bool UInstanceCubeVolume::CreateNearInstanceCubeVolumeUnit(FPieceLocation location,FInstanceCubeVolumeUnit& nearVolumeUnit)
{
	if(GetIsInSide(location))
	{
		return false;
	}

	if(location.Floor >= MinFloor && location.Floor <= MaxFloor)
	{
		nearVolumeUnit.Location = FPieceLocation(location,MinFloor);
		nearVolumeUnit.Volume = FPieceLocation(1,1,FloorHeight);
		return true;
	}

	return false;
}

bool UInstanceCubeVolume::AddVolume(FPieceLocation location,FPieceLocation volume)
{
	bool isEnableAdd = false;
	FInstanceCubeVolumeUnit addVolumeUnit(location,volume);
	if(Volumes.Num() == 0)
	{
		Volumes.Add(addVolumeUnit);
		FloorHeight = volume.Floor;

		MinFloor = location.Floor;
		MaxFloor = MinFloor + FloorHeight;

		isEnableAdd = true;
	}
	else
	{
		for(int index = 0;index<Volumes.Num();++index)
		{
			FInstanceCubeVolumeUnit& curVolume = Volumes[index];
			if(curVolume.GetEnableBind( addVolumeUnit))
			{
				isEnableAdd = true;
				break;
			}
		}
		bool isMerge=false;
		for(int index = 0;index<Volumes.Num();++index)
		{
			FInstanceCubeVolumeUnit& curVolume = Volumes[index];
			if(curVolume.TryMerge(addVolumeUnit))
			{
				isMerge = true;
				break;
			}
		}
		if(isEnableAdd && !isMerge)
		{
			Volumes.Add(addVolumeUnit);	
		}
	}
	return isEnableAdd;
}

bool UInstanceCubeVolume::GetIsInSide(FPieceLocation location)
{
	for(int index = 0;index<Volumes.Num();++index)
	{
		FInstanceCubeVolumeUnit& curVolume = Volumes[index];
		if(curVolume.GetIsInSide(location))
		{
			return true;
		}
	}

	return false;
}

bool UInstanceCubeVolume::GetIsInEdge(FPieceLocation location)
{
	for(int index = 0;index<Volumes.Num();++index)
	{
		FInstanceCubeVolumeUnit& curVolume = Volumes[index];
		if(curVolume.GetIsInEdge(location))
		{
			return true;
		}
	}

	return false;
}

TArray<FInstanceCubeVolumeUnit> UInstanceCubeVolume::GetVolumes()
{
	return Volumes;
}

int UInstanceCubeVolume::GetFloorHeight()
{
	return FloorHeight;
}
