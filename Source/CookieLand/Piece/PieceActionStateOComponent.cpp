// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceActionStateOComponent.h"

#include "BasePiece.h"
#include "BasePieceActor.h"
#include "BasePieceLand.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"

void UPieceActionStateOComponent::SetPiece(class UBasePiece* piece)
{
	Piece = piece;
}

void UPieceActionStateOComponent::Init()
{
	
}

void UPieceActionStateOComponent::UnInit()
{
	
}

void UPieceActionStateOComponent::AddState(FAddPieceActionStateRequest addRequest,class UPieceBaseActionRunTimeInfo* actionInfo)
{
	if(addRequest.State == EPieceActionState::Empty)
	{
		return;
	}
	
	if(GetIsAttachState(addRequest.State))
	{
		return;
	}
	
	if(addRequest.IsReqMainState && EnableReplaceMainState)
	{
		MainState = addRequest.State;
		EnableReplaceMainState = addRequest.EnableReplaceMainState;
	}

	AttachStates.Add(addRequest.State);

	if(actionInfo)
	{
		ActionStateRunTimeInfos.Add(addRequest.State,actionInfo);
	}
}

void UPieceActionStateOComponent::RemoveState(EPieceActionState actionState)
{
	if(actionState == EPieceActionState::Empty)
	{
		return;
	}
	
	if(GetIsAttachState(actionState))
	{
		return;
	}

	AttachStates.Remove(actionState);
	
	if(MainState == actionState)
	{
		if(AttachStates.Num()>0)
		{
			MainState = AttachStates[0];
		}
		else
		{
			MainState = EPieceActionState::Empty;
		}
	}
}

bool UPieceActionStateOComponent::GetIsAttachState(EPieceActionState actionState)
{
	for(int index = 0;index<AttachStates.Num();++index)
	{
		if(AttachStates[index] == actionState)
		{
			return true;
		}
	}
	return false;
}

void UPieceActionStateOComponent::TriggerAction_RemindDropOut()
{
	ABasePieceActor* pieceActor = Piece->GetPieceActor();
	if(pieceActor)
	{
		pieceActor->TriggerActionArt_RemindDropOut();
	}
}

void UPieceActionStateOComponent::TriggerAction_DropOut()
{
	ABasePieceActor* pieceActor = Piece->GetPieceActor();
	if(pieceActor)
	{
		pieceActor->TriggerActionArt_DropOut();
	}
	else
	{
		Piece->GetOwnLand()->DeletePieceById(Piece->GetId());
	}
}

void UPieceActionStateOComponent::TriggerAction_Rotation(bool isVertical,float angle)
{
	bool enableRotation = false;

	if(isVertical)
	{
		if(Piece->GetEnableMove(EPieceDirection::Up) || Piece->GetEnableMove(EPieceDirection::Down))
		{
			enableRotation = true;
			if(Piece->GetEnableMove(EPieceDirection::Up))
			{
				Piece->OverlaySetEnableMove({EPieceDirection::Down});
			}
			else
			{
				Piece->OverlaySetEnableMove({EPieceDirection::Up});
			}
		}
	}
	else
	{
		TArray<EPieceDirection> directions{EPieceDirection::Left,EPieceDirection::Right,EPieceDirection::Forward,EPieceDirection::Backward};
		TArray<EPieceDirection> changeDirections;
		for(int index = 0;index<directions.Num();++index)
		{
			EPieceDirection sourceDirection = directions[index];
			if(!Piece->GetEnableMove(sourceDirection))
			{
				continue;
			}
			EPieceDirection changeDirection = UCommonFunctionLibrary::ChangeGameDirectionByAngle(angle,sourceDirection);
			changeDirections.Add(changeDirection);
		}

		enableRotation = changeDirections.Num() != 4;

		if(enableRotation)
		{
			Piece->OverlaySetEnableMove(changeDirections);
		}
	}

	if(!enableRotation)
	{
		return;
	}
	
	ABasePieceActor* pieceActor = Piece->GetPieceActor();
	if(pieceActor)
	{
		pieceActor->TriggerActionArt_Rotation(isVertical,angle);
	}
}