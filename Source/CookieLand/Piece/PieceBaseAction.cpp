// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceBaseAction.h"

#include "BasePiece.h"
#include "BasePieceLand.h"
#include "PieceLandSystem.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"

void UPieceBaseAction::SetHandle(FPieceActionHandle handle)
{
	Handle = handle;
}

void UPieceBaseAction::SetPiece(class UBasePiece* piece)
{
	Piece = piece;
	PieceId = piece->GetId();
}

void UPieceBaseAction::SetData(class UPieceBaseActionConfigData* data)
{
	BaseConfigData = data;
}

void UPieceBaseAction::Init()
{
	isFinish = false;
	_Init();
}

void UPieceBaseAction::_Init(){}

void UPieceBaseAction::Execute(){}

bool UPieceBaseAction::IsFinish()
{
	return isFinish;
}

void UPieceBaseAction::UnInit()
{
	_UnInit();
}

void UPieceBaseAction::_UnInit(){}

void UPieceBaseAction::SetIsFinish(bool value)
{
	isFinish = value;

	if(isFinish && BaseConfigData && BaseConfigData->IsAutoFinish )
	{
		UPieceLandSystem* pieceLandSystem = UCommonFunctionLibrary::GetPieceLandSystem();
		if(UBasePieceLand* land = pieceLandSystem->GetCurLand())
		{
			land->DeleteActionByPiece(Handle);
		}
	}
}

FPieceActionHandle UPieceBaseAction::GetHandle()
{
	return Handle;
}