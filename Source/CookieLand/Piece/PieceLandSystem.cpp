// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceLandSystem.h"

#include "BasePiece.h"
#include "BasePieceLand.h"
#include "PieceBaseAction.h"
#include "PieceTypes.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/Gameplay/CookieLandGameInstance.h"
#include "Engine/DataTable.h"


bool UPieceLandSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UPieceLandSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UCookieLandGameInstance* gameInstance = UCommonFunctionLibrary::GetGameInstance(GetWorld());
	if(gameInstance)
	{
		LevelLandDataTable = gameInstance->LevelLandDataTable;
		PieceCardCollectionDataAsset = gameInstance->PieceCardCollectionDataAsset;
	}
}

void UPieceLandSystem::BeginDestroy()
{
	Super::BeginDestroy();
}

void UPieceLandSystem::Deinitialize()
{
	
}

void UPieceLandSystem::CreateLevelLand(FName levelName,bool forceCreate)
{
	if(!LevelLandDataTable)
	{
		return;
	}

	if(!forceCreate && CurLand && CurLand->GetLevelName() == levelName)
	{
		return;
	}
	
	FLevelLandDataTable* row = LevelLandDataTable->FindRow<FLevelLandDataTable>(levelName,TEXT("FLevelLandDataTable"));
	if(!row)
	{
		return;
	}

	if(CurLand)
	{
		CurLand->DestroyLand();
		CurLand = nullptr;
	}

 	CurLand = NewObject<UBasePieceLand>(this);
	CurLand->CreateLand(levelName,row->LandDataAsset);

	CreatePieceLandEvent.Broadcast(levelName,row->LandDataAsset->InitialPieceId);
}

void UPieceLandSystem::AgainCurLevelLand()
{
	if(CurLand)
	{
		CreateLevelLand(CurLand->GetLevelName(),true);
	}
}

UBasePieceLand* UPieceLandSystem::GetCurLand()
{
	return CurLand;
}

void UPieceLandSystem::ToNextLand()
{
	if(!LevelLandDataTable || !CurLand)
	{
		return;
	}

	FName curLevelName = CurLand->GetLevelName();
	FLevelLandDataTable* curLandConfig = LevelLandDataTable->FindRow<FLevelLandDataTable>(curLevelName,TEXT("FLevelLandDataTable"));
	if(!curLandConfig)
	{
		return;
	}

	if(curLandConfig->IsNextInOrder)
	{
		TArray<FName> levelNames;
		LevelLandDataTable->GetRowMap().GetKeys(levelNames);

		for(int index = 0;index<levelNames.Num();++index)
		{
			if(levelNames[index] == curLevelName)
			{
				if(index != levelNames.Num()-1)
				{
					CreateLevelLand(levelNames[index+1]);
					break;
				}
			}
		}
	}
	else
	{
		CreateLevelLand(curLandConfig->NextTargetLevel);
	}
}

TArray<FName> UPieceLandSystem::GetAllPieceCardNames()
{
	TArray<FName> names;

	if(PieceCardCollectionDataAsset)
	{
		for(int index = 0;index<PieceCardCollectionDataAsset->Cards.Num();++index)
		{
			FPieceCardConfigData cardConfigData = PieceCardCollectionDataAsset->Cards[index];
			names.Add(cardConfigData.CardName);
		}
	}

	return names;
}

bool UPieceLandSystem::GetPieceCardConfigData(FName cardName,FPieceCardConfigData& pieceCardConfigData)
{
	for(int index = 0;index<PieceCardCollectionDataAsset->Cards.Num();++index)
	{
		FPieceCardConfigData cardConfigData = PieceCardCollectionDataAsset->Cards[index];
		if(cardConfigData.CardName != cardName)
		{
			continue;
		}

		pieceCardConfigData = cardConfigData;
		return true;
	}
	return false;
}