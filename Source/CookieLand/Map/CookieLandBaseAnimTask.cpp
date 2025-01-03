// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/CookieLandBaseAnimTask.h"
#include "CookieLandBasePieceAction.h"
#include "CookieLand/Map/CookieLandPiece.h"
#include "CookieLand/Map/CookieLandMapBuildActor.h"
#include "CookieLand/Map/CookieLandMapActorGather.h"

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
	bIsActive = true;

	if (!GetPiece())
	{
		Finish();
		return;
	}

	if (ActiveEvent.IsBound())
	{
		ActiveEvent.Broadcast();
	}
}

void UCookieLandBaseAnimTask::Finish()
{
	if (FinishEvent.IsBound())
	{
		FinishEvent.Broadcast();
	}

	if (Action)
	{
		Action->RemoveTaskById(Id);
	}

	bIsFinish = true;
}

void UCookieLandBaseAnimTask::UnInit()
{
	Action = nullptr;
	Piece = nullptr;
}

UCookieLandBaseAnimTask* UCookieLandBaseAnimTask::CreateAnimTask(UCookieLandBasePieceAction* ThisAction, TSubclassOf< UCookieLandBaseAnimTask> TaskType)
{
	UCookieLandBaseAnimTask* NewTask = NewObject<UCookieLandBaseAnimTask>(ThisAction, TaskType);

	ThisAction->AddTask(NewTask);

	return NewTask;
}

int UCookieLandBaseAnimTask::GetId()
{
	return Id;
}

UCookieLandBasePieceAction* UCookieLandBaseAnimTask::GetAction()
{
	return Action.Get();
}

UCookieLandPiece* UCookieLandBaseAnimTask::GetPiece()
{
	return Piece.Get();
}

ACookieLandPieceActor* UCookieLandBaseAnimTask::GetPieceActor()
{
	if (Piece)
	{
		return Piece->GetPieceActor();
	}

	return nullptr;
}

bool UCookieLandBaseAnimTask::GetIsActiving()
{
	return bIsActive && !bIsFinish;
}