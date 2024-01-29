// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePiece.h"

#include "BasePieceActor.h"
#include "BasePieceLand.h"
#include "PieceTypes.h"

// Sets default values
UBasePiece::UBasePiece(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CurInfo = NewObject<UPieceInfo>();
	CurInfo->Info = NewObject<UPieceBaseInfo>();
}

void UBasePiece::SetId(int Id)
{
	CurInfo->Id = Id;
}

int UBasePiece::GetId()
{
	return CurInfo->Id;
}

void UBasePiece::SetOwnLand(class UBasePieceLand* land)
{
	OwnLand = land;
}

class UBasePieceLand* UBasePiece::GetOwnLand()
{
	return OwnLand;
}

void UBasePiece::SetConfigData(class UPieceBaseConfigData* configData)
{
	ConfigData = configData;
}

void UBasePiece::Init()
{
	CurInfo->IsOccupy = false;
	CurInfo->Info->Copy(ConfigData->BaseInfo);

	if(OwnLand->RequestOccupyLocation(CurInfo->Id,CurInfo->Info->Location))
	{
		CurInfo->IsOccupy = true;
	}
	else
	{
		CurInfo->Info->Location.IsValid = false;
	}

	if(CurInfo->IsOccupy)
	{
		FTransform transform;
		transform.SetLocation(OwnLand->GetActorLocationByOccupyLocation(CurInfo->Info->Location));
	
		FActorSpawnParameters spawnParameters;
		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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
	}
}

void UBasePiece::UnInit()
{
	if(PieceActor)
	{
		PieceActor->Destroy();
	}

	if(OwnLand && CurInfo->IsOccupy)
	{
		OwnLand->RequestUnOccupyLocation(CurInfo->Id,CurInfo->Info->Location);
	}
}

const UPieceInfo* UBasePiece::GetCurInfo()
{
	return CurInfo;
}

const UPieceBaseConfigData* UBasePiece::GetConfigData()
{
	return ConfigData;
}

bool UBasePiece::GetEnableMove(EPieceDirection direction)
{
	for(int index = 0;index<CurInfo->Info->EnableDirections.Num();++index)
	{
		if(CurInfo->Info->EnableDirections[index] == direction)
		{
			return true;
		}
	}

	return false;
}
