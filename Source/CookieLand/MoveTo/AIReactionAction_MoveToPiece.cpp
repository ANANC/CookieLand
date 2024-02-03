// Fill out your copyright notice in the Description page of Project Settings.


#include "AIReactionAction_MoveToPiece.h"

#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/Piece/BasePieceLand.h"

void UAIReactionAction_MoveToPiece::SetActionData(UAIReactionBaseMoveToActionData* data)
{
	Super::SetActionData(data);
	MoveToPieceActionData = Cast<UAIReactionMoveToPieceActionData>(data);
}

void UAIReactionAction_MoveToPiece::CalculateMoveToPath()
{
	if(ShowDebugLine)
	{
		UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove. CalculateMoveToPath. MoveToPiece"),*CharacterInstanceName);
	}
	
	CalculatePathLocations.Reset();
	
	UBasePieceLand* pieceLand = UCommonFunctionLibrary::GetCurPieceLand();
	if(pieceLand)
	{
		UBasePiece* piece = pieceLand->GetPieceById(MoveToPieceActionData->TargetPieceId);
		if(piece)
		{
			FVector location = pieceLand->GetActorLocationById(MoveToPieceActionData->TargetPieceId);
			CalculatePathLocations.Add(location);
		}
	}
}
