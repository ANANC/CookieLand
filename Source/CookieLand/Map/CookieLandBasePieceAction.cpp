// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandBasePieceAction.h"
#include "CookieLandPiece.h"
#include "CookieLandMapBuildActor.h"
#include "CookieLandMapBuilder.h"

void UCookieLandBasePieceAction::Init(int InId, UCookieLandPiece* InPiece, UCookieLandBasePieceActionData* InData)
{
	Id = InId;
	Piece = InPiece;
	BaseData = InData;
}

void UCookieLandBasePieceAction::SetData(UCookieLandBasePieceActionData* InData){}

void UCookieLandBasePieceAction::UnInit()
{
	Id = -1;
	Piece = nullptr;
}

void UCookieLandBasePieceAction::Active()
{

}

void UCookieLandBasePieceAction::Finish()
{
	if (Piece)
	{
		Piece->RemovePieceActionById(Id);
	}
}

int UCookieLandBasePieceAction::GetId() const
{
	return Id;
}

UCookieLandPiece* UCookieLandBasePieceAction::GetPiece()
{
	return Piece.Get();
}

ACookieLandMapBuildActor* UCookieLandBasePieceAction::GetMapBuildActor()
{
	if (Piece)
	{
		return Piece->GetMapBuildActor();
	}
	return nullptr;
}

UCookieLandMapBuilder* UCookieLandBasePieceAction::GetMapBuilder()
{
	ACookieLandMapBuildActor* MapBuildActor = GetMapBuildActor();
	if (MapBuildActor)
	{
		return MapBuildActor->GetMapBuilder();
	}
	return nullptr;
}

const UCookieLandBasePieceActionData* UCookieLandBasePieceAction::GetData() const
{
	return BaseData;
}