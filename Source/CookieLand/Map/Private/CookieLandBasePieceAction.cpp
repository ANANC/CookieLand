// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Public/CookieLandBasePieceAction.h"

void UCookieLandBasePieceAction::Init(int InId, UCookieLandPiece* InPiece, UCookieLandBasePieceActionData* InData)
{
	Id = InId;
	Piece = InPiece;
	Data = InData;
}

void UCookieLandBasePieceAction::UnInit()
{
	Id = -1;
	Piece = nullptr;
}

void UCookieLandBasePieceAction::Active()
{

}

int UCookieLandBasePieceAction::GetId() const
{
	return Id;
}

UCookieLandPiece* UCookieLandBasePieceAction::GetPiece()
{
	return Piece.Get();
}

const UCookieLandBasePieceActionData* UCookieLandBasePieceAction::GetData() const
{
	return Data;
}