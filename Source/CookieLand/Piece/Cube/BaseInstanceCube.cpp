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

		
	}

	return false;
}

bool UBaseInstanceCube::AddVolume(FPieceLocation location)
{
	return false;
}