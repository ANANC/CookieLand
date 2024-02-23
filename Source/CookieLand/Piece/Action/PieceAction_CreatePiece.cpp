// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceAction_CreatePiece.h"

#include "CookieLand/Character/BaseCharacter.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/Piece/BasePiece.h"
#include "CookieLand/Piece/BasePieceLand.h"
#include "CookieLand/Piece/PieceLandComponent.h"

void UPieceAction_CreatePiece::_Init()
{
	if(!ConfigData)
	{
		SetIsFinish(true);
		return;
	}
}
	
void UPieceAction_CreatePiece::_UnInit()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(GetWorld());
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		pieceLandComponent->MoveToNextPieceEvent.RemoveAll(this);
	}
}

void UPieceAction_CreatePiece::SetData(class UPieceBaseActionConfigData* data)
{
	Super::SetData(data);
	ConfigData = Cast<UPieceActionConfigData_CreatePiece>(data);
}

void UPieceAction_CreatePiece::Execute()
{
	if(ConfigData->TriggerType == EPieceActionCreatePieceTriggerType::Immediately)
	{
		TriggerCreatePiece();
	}
	else 
	{
		if(!Piece)
		{
			SetIsFinish(true);
			return;
		}
		
		ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(GetWorld());
		if(!mainCharacter)
		{
			SetIsFinish(true);
			return;
		}

		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		pieceLandComponent->MoveToNextPieceEvent.AddDynamic(this,&UPieceAction_CreatePiece::MoveToNextPieceEventCallback);

		MoveToNextPieceEventCallback(pieceLandComponent->GetLastLocation(),pieceLandComponent->GetCurLocation());
	}
}

void UPieceAction_CreatePiece::MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation)
{
	int characterStandPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation(newLocation,characterStandPieceId);

	if(ConfigData->TriggerType == EPieceActionCreatePieceTriggerType::EnterPiece && characterStandPieceId == Piece->GetId())
	{
		TriggerCreatePiece();
	}
}

void UPieceAction_CreatePiece::TriggerCreatePiece()
{
	UBasePieceLand* Land = Piece?Piece->GetOwnLand():UCommonFunctionLibrary::GetCurPieceLand();
	if(!Land)
	{
		SetIsFinish(true);
		return;
	}

	FPieceLocation createLocation = GetCreateLocation();

	do
	{
		int pieceId;
		if(Land->GetPieceIdByLocation(createLocation,pieceId))
		{
			if(!ConfigData->IsForceReplace)
			{
				break;
			}

			Land->DeletePieceById(pieceId);
		}

		CreatePiece(createLocation);
		
	}while (false);
	
	SetIsFinish(true);
}

FPieceLocation UPieceAction_CreatePiece::GetCreateLocation()
{
	UPieceLandComponent* pieceLandComponent=nullptr;
	if(ABaseCharacter* character = UCommonFunctionLibrary::GetMainCharacter(GetWorld()))
	{
		pieceLandComponent = character->GetPieceLandComponent();
	}
	
	FPieceLocation location;
	
	switch(ConfigData->LocationType)
	{
	case EPieceActionCreatePieceLocationType::BaseOnCharacter:
		if(pieceLandComponent)
		{
			location = pieceLandComponent->GetCurLocation();
			location += ConfigData->RelativeCreateLocation;
		}
		else
		{
			location =  FPieceLocation(false);
		}
		break;
	case EPieceActionCreatePieceLocationType::BaseOnCurLocation:
		if(Piece)
		{
			location = Piece->GetCurInfo()->Info->Location;
			location += ConfigData->RelativeCreateLocation;
		}
		else
		{
			location = TriggerLocation;
			location += ConfigData->RelativeCreateLocation;
		}
		break;
	case EPieceActionCreatePieceLocationType::TargetLocation:
		location = ConfigData->TargetCreateLocation;
		break;
	default:
		location =  FPieceLocation(false);
		break;
	}
	
	return location;
}

void UPieceAction_CreatePiece::CreatePiece(FPieceLocation location)
{
	UBasePieceLand* Land = Piece?Piece->GetOwnLand():UCommonFunctionLibrary::GetCurPieceLand();
	if(!Land)
	{
		return;
	}
	Land->CreateDynamicPieceByLocation(location,ConfigData->IsUseDefaultPieceConfig,ConfigData->PieceConfig);
}