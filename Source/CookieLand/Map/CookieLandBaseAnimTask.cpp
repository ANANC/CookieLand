// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/CookieLandBaseAnimTask.h"
#include "CookieLand/Map/Public/CookieLandBasePieceAction.h"

void UCookieLandBaseAnimTask::Init(int InId, UCookieLandBasePieceAction* InAction)
{
	Id = InId;
	Action = InAction;
	if (Action)
	{
		Piece = Action->GetPiece();
	}
}

void UCookieLandBaseAnimTask::Active()
{

}

void UCookieLandBaseAnimTask::Finish()
{

}

int UCookieLandBaseAnimTask::GetId()
{
	return Id;
}

UCookieLandBasePieceAction* UCookieLandBaseAnimTask::GetAction()
{
	return Action;
}

UCookieLandPiece* UCookieLandBaseAnimTask::GetPiece()
{
	return Piece;
}