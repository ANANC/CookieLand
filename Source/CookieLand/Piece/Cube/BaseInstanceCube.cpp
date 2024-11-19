// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInstanceCube.h"

#include "CookieLand/Piece/BasePiece.h"
#include "CookieLand/Piece/BasePieceLand.h"

void UBaseInstanceCube::SetCubeId(int id)
{
	CubeId = id;
}

void UBaseInstanceCube::SetOwnLand(class UBasePieceLand* land)
{
	OwnLand = land;
}

int UBaseInstanceCube::GetCubeId()
{
	return CubeId;	
}
	
UBasePieceLand* UBaseInstanceCube::GetOwnLand()
{
	return OwnLand;
}

void UBaseInstanceCube::Init()
{
	CubeVolume = NewObject<UInstanceCubeVolume>();
}

void UBaseInstanceCube::UnInit()
{
	
}

bool UBaseInstanceCube::CreateCube(FPieceLocation location)
{
	int pieceId;
	if(OwnLand->GetPieceIdByLocation(location,pieceId))
	{
		UBasePiece* piece = OwnLand->GetPieceById(pieceId);

		bool isEnable,isUp;
		piece->GetIsEnableUpOrDownMove(isEnable,isUp);
		if(!isEnable)
		{
			return false;
		}
		UBasePiece* nextPiece = OwnLand->GetNearPieceByUpOrDown(pieceId,piece->GetCurInfo()->Info->Distance,isUp?EPieceDirection::Up:EPieceDirection::Down);
		if(!nextPiece)
		{
			return false;
		}

		int floor = abs(piece->GetLocation().Floor - nextPiece->GetLocation().Floor);
		
		FPieceLocation volumeLocation = piece->GetLocation().Floor < nextPiece->GetLocation().Floor?piece->GetLocation():nextPiece->GetLocation();
		FPieceLocation volume(1,1,floor);
		
		if(!CubeVolume->AddVolume(volumeLocation,volume))
		{
			return false;
		}
		
		CreateCubeActor();
		
		return true;
	}

	return false;
}

bool UBaseInstanceCube::AddVolume(FPieceLocation location)
{
	if(CubeVolume)
	{
		FInstanceCubeVolumeUnit nearVolumeUnit;
		if(CubeVolume->CreateNearInstanceCubeVolumeUnit(location,nearVolumeUnit))
		{
			TArray<FPieceLocation> sideLocations;
			sideLocations.Add(FPieceLocation(nearVolumeUnit.Location));
			sideLocations.Add(FPieceLocation(nearVolumeUnit.Location,nearVolumeUnit.Volume.Floor));

			for(int index = 0;index<sideLocations.Num();++index)
			{
				int pieceId;
				if(!OwnLand->GetPieceIdByLocation(sideLocations[index],pieceId))
				{
					return false;
				}
			}

			if(!CubeVolume->AddVolume(nearVolumeUnit.Location,nearVolumeUnit.Volume))
			{
				return false;
			}


			return true;
		}
	}
	return false;
}

bool UBaseInstanceCube::GetIsInSide(FPieceLocation location)
{
	if(CubeVolume)
	{
		return CubeVolume->GetIsInSide(location);
	}
	return false;
}

bool UBaseInstanceCube::GetIsInEdge(FPieceLocation location)
{
	if(CubeVolume)
	{
		return CubeVolume->GetIsInEdge(location);
	}
	return false;
}

void UBaseInstanceCube::CreateCubeActor()
{
	if(CubeActor)
	{
		return;
	}

	/*
	TSubclassOf<class ABasePieceActor> actorType = OwnLand->GetPieceInstanceActorClass(this);
	if(actorType)
	{
		AActor* actor = GetWorld()->SpawnActor(actorType, &transform, spawnParameters);
		if(actor)
		{
			PieceActor = Cast<ABasePieceActor>(actor);
			PieceActor->SetOwnLand(OwnLand);
			PieceActor->SetPiece(this);
			PieceActor->Init();
		}
	}
	*/
}