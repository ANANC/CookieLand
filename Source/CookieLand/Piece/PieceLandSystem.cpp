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

bool UPieceLandSystem::CreateActionToPiece(FPieceActionHandle& handle,int pieceId,class UPieceBaseActionConfigData* actionData)
{
	if(!actionData || !actionData->ActionClass || !CurLand)
	{
		return false;
	}
	
	UBasePiece* piece = CurLand->GetPieceById(pieceId);
	if(!piece)
	{
		return false;
	}

	UPieceBaseAction* action = NewObject<UPieceBaseAction>(Cast<UObject>(piece),actionData->ActionClass);
	if(!action)
	{
		return false;
	}

	handle = FPieceActionHandle(actionAutoId,pieceId);
	
	actionAutoId+=1;
	
	action->SetHandle(handle);
	action->SetData(actionData);
	action->SetPiece(piece);
	action->Init();

	piece->AddAction(action);

	return true;
}

void UPieceLandSystem::DeleteActionByPiece(FPieceActionHandle handle)
{
	if(!handle.GetIsValid())
	{
		return;
	}

	do
	{
		if(!CurLand)
		{
			break;
		}
		
		UBasePiece* piece = CurLand->GetPieceById(handle.GetPieceId());
		if(!piece)
		{
			break;
		}

		piece->RemoveAction(handle);
	}
	while (false);
	
	handle.Clear();
}