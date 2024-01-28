// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceLandSystem.h"

#include "BasePieceLand.h"
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
	}
}

void UPieceLandSystem::BeginDestroy()
{
	Super::BeginDestroy();
}

void UPieceLandSystem::Deinitialize()
{
	
}

void UPieceLandSystem::CreateLevelLand(FName levelName)
{
	if(!LevelLandDataTable)
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

UBasePieceLand* UPieceLandSystem::GetCurLand()
{
	return CurLand;
}