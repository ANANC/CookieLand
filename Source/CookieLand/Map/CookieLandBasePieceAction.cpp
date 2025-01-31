// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandBasePieceAction.h"
#include "CookieLandPiece.h"
#include "CookieLandMapBuildActor.h"
#include "CookieLandMapBuilder.h"
#include "CookieLandBaseAnimTask.h"
#include "CookieLandBaseCueActor.h"
#include "CookieLand/Map/CookieLandMapBuildLibrary.h"

void UCookieLandBasePieceAction::Init(int InId, UCookieLandPiece* InPiece, UCookieLandBasePieceActionData* InData)
{
	Id = InId;
	Piece = InPiece;
	BaseData = InData;
	SetData(BaseData);
}

void UCookieLandBasePieceAction::SetData(UCookieLandBasePieceActionData* InData){}

void UCookieLandBasePieceAction::Active()
{
	bActiving = true;
}

void UCookieLandBasePieceAction::Finish()
{
	if (Piece)
	{
		Piece->RemovePieceActionById(Id);
	}

	bActiving = false;
}

bool UCookieLandBasePieceAction::GetActiving()
{
	return bActiving;
}

void UCookieLandBasePieceAction::UnInit()
{
	Id = -1;
	Piece = nullptr;
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

void UCookieLandBasePieceAction::AddTask(UCookieLandBaseAnimTask* InTask)
{
	if (!InTask)
	{
		return;
	}

	Tasks.Add(InTask);

	AutoTaskId += 1;
	InTask->Init(AutoTaskId, this);
}

void UCookieLandBasePieceAction::RemoveTaskById(int InTaskId)
{
	for (int Index = 0; Index < Tasks.Num(); ++Index)
	{
		TObjectPtr<UCookieLandBaseAnimTask> Task = Tasks[Index];
		if (Task->GetId() == InTaskId)
		{
			Tasks.RemoveAt(Index);
			Task->UnInit();
			break;
		}
	}
}

ACookieLandBaseCueActor* UCookieLandBasePieceAction::CreateCueActor(UCookieLandBasePieceAction* InPieceAction, FGameplayTag CueActorTag, UCookieLandBaseCueActorData* InBaseData)
{
	if (!InPieceAction)
	{
		return nullptr;
	}

	FCueActorData OutCueActorData;
	if (InPieceAction->GetCueActorDataByTag(CueActorTag, OutCueActorData))
	{
		return CreateCueActor(InPieceAction,OutCueActorData.CueActorType, InBaseData? InBaseData:OutCueActorData.CueActorData);
	}

	return nullptr;
}

ACookieLandBaseCueActor* UCookieLandBasePieceAction::CreateCueActor(UCookieLandBasePieceAction* InPieceAction, TSubclassOf<ACookieLandBaseCueActor> CueActorType, UCookieLandBaseCueActorData* InBaseData)
{
	if (!CueActorType || !InPieceAction)
	{
		return nullptr;
	}

	ACookieLandPieceActor* PieceActor = InPieceAction->GetPiece()->GetPieceActor();
	if (!PieceActor)
	{
		return nullptr;
	}

	UWorld* World = InPieceAction->GetPiece()->GetPieceActor()->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FTransform InitTransfrom;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (InBaseData)
	{
		if (InBaseData->bAttachPieceActor)
		{
			SpawnParameters.Owner = PieceActor;
			SpawnParameters.Instigator = PieceActor->GetInstigator();
		}
	}

	AActor* InstanceActor = World->SpawnActor(CueActorType, &InitTransfrom, SpawnParameters);
	ACookieLandBaseCueActor* CueActor = Cast<ACookieLandBaseCueActor>(InstanceActor);

	if (InBaseData)
	{
		if (InBaseData->bAttachPieceActor)
		{
			CueActor->AttachToActor(PieceActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

	InPieceAction->AddCueActor(CueActor, InBaseData);

	return CueActor;
}

void UCookieLandBasePieceAction::AddCueActor(ACookieLandBaseCueActor* InCueActor, UCookieLandBaseCueActorData* InCueActorData)
{
	CueActors.Add(InCueActor);

	InCueActor->Init(this, InCueActorData);
}

void UCookieLandBasePieceAction::DestroyCueActor(UCookieLandBasePieceAction* InPieceAction, ACookieLandBaseCueActor* InCueActor)
{
	if (!InPieceAction || !InCueActor)
	{
		return;
	}

	InPieceAction->RemoveCueActor(InCueActor);
	InCueActor->Destroy();
}

void UCookieLandBasePieceAction::RemoveCueActor(ACookieLandBaseCueActor* InCueActor)
{
	InCueActor->UnInit();

	CueActors.Remove(InCueActor);
}

bool UCookieLandBasePieceAction::GetCueActorDataByTag(FGameplayTag CueActorTag, FCueActorData& OutCueActorData)
{
	if (BaseData)
	{
		for (int Index = 0; Index < BaseData->CueActorDatas.Num(); ++Index)
		{
			FCueActorData CueActorData = BaseData->CueActorDatas[Index];
			if (CueActorData.CueActorTag == CueActorTag)
			{
				OutCueActorData = CueActorData;
				return true;
			}
		}
	}
	return false;
}