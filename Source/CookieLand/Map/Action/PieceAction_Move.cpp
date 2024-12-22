// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Action/PieceAction_Move.h"
#include "CookieLand/Map/Public/CookieLandPiece.h"

#pragma region UPieceActionData_Move

void UPieceActionData_Move::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.MemberProperty)
	{
		const FName MemberPropertyName = PropertyChangedEvent.MemberProperty->GetFName();
		if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UPieceActionData_Move, Trigger))
		{
			bActiveImmediately = Trigger == EPieceActionMoveTriggerType::Auto;
		}
	}
}

#pragma endregion


void UPieceAction_Move::SetData(UCookieLandBasePieceActionData* InData)
{
	Data = Cast< UPieceActionData_Move>(InData);
}

void UPieceAction_Move::Active()
{
	StartLocator = Piece->GetPieceLocator();
	EndLocator = GetMoveTarget();
}

FCookieLandPieceLocator UPieceAction_Move::GetMoveTarget()
{
	if (Data->MoveType == EPieceActionMoveType::NextPiece)
	{
		Piece->get
	}
	else if (Data->MoveType == EPieceActionMoveType::Fixed)
	{

	}
}