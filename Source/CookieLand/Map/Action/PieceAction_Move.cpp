// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Action/PieceAction_Move.h"
#include "CookieLand/Map/CookieLandPiece.h"
#include "CookieLand/Map/CookieLandMapBuildActor.h"
#include "CookieLand/Map/CookieLandMapBuilder.h"
#include "CookieLand/Map/Task/AnimTask_Move.h"
#include "CookieLand/Map/CookieLandMapBuildLibrary.h"
#include "CookieLand/PerceptualObject/CookieLandPerceptualObjectSubsystem.h"

#pragma region UPieceAction_Move
void UPieceAction_Move::SetData(UCookieLandBasePieceActionData* InData)
{
	Data = Cast<UPieceActionData_Move>(InData);
	check(Data);
}

void UPieceAction_Move::Active()
{
	Super::Active();

	MoveOrientation = Data->MoveOrientation;

	StartLocator = Piece->GetPieceLocator();
	EndLocator = GetMoveTarget();

	if (StartLocator == EndLocator)
	{
		UCookieLandMapBuilder* MapBuilder = GetMapBuilder();
		if(MapBuilder)
		{
			MapBuilder->MapLocatorOccupyStateChangeEvent.AddDynamic(this, &UPieceAction_Move::ReceiveMapLocatorOccupyStateChangeEventCallback);
		}
		else
		{
			Finish();
		}
		return;
	}

	TryTrigger(Data->Trigger);
}

void UPieceAction_Move::Finish()
{
	UCookieLandMapBuilder* MapBuilder = GetMapBuilder();
	if (MapBuilder)
	{
		MapBuilder->MapLocatorOccupyStateChangeEvent.RemoveDynamic(this, &UPieceAction_Move::ReceiveMapLocatorOccupyStateChangeEventCallback);
	}
	UCookieLandPieceLibrary::RemoveActiveGameplayTag_Moving(Piece);

	if (MoveTask)
	{
		MoveTask->FinishEvent.RemoveDynamic(this, &UPieceAction_Move::MoveTaskFinishEventCallback);
	}
	DestroyCueActor(this,CueActor_Move);

	Super::Finish();
}

FCookieLandPieceLocator UPieceAction_Move::GetMoveTarget()
{
	FCookieLandPieceLocator Locator = StartLocator;
	if (Data->MoveType == EPieceActionMoveType::NextPiece)
	{
		UCookieLandMapBuilder* MapBuilder = GetMapBuilder();
		if (MapBuilder)
		{
			FCookieLandPieceLocator NearestLocator;
			if (MapBuilder->GetNearestPieceLocator(StartLocator, MoveOrientation, Locator))
			{
				Locator = NearestLocator;
			}
		}
	}
	else if (Data->MoveType == EPieceActionMoveType::Fixed)
	{
		Locator.PieceLocation.AddDistanceBySixDirection(MoveOrientation, Data->FixedDistance);
		if (!Data->bIngoreObstacle)
		{
			UCookieLandMapBuilder* MapBuilder = GetMapBuilder();
			if (MapBuilder)
			{
				FCookieLandPieceLocator NearestLocator;
				if (MapBuilder->GetNearestPieceLocator(StartLocator, Data->MoveOrientation, NearestLocator))
				{
					if (NearestLocator != Locator)
					{
						TMap<ECookieLandPieceOrientation, int> RelativeOrientationAndDistances = StartLocator.PieceLocation.GetRelativeOrientationAndDistances(NearestLocator.PieceLocation);
						if (RelativeOrientationAndDistances.Contains(Data->MoveOrientation))
						{
							float Distance = RelativeOrientationAndDistances[Data->MoveOrientation];
							if (Distance < Data->FixedDistance)
							{
								Locator = NearestLocator;
							}
						}
					}
				}
			}
		}
	}
	return Locator;
}

void UPieceAction_Move::TryTrigger(EPieceActionMoveTriggerType InTriggerType)
{
	if (InTriggerType == EPieceActionMoveTriggerType::Auto)
	{
		PlayMoveTask();
	}
	else
	{
		UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();

		if (PerceptualObjectSubsystem->HasPerceptualObjectStandByTargetLocation(StartLocator))
		{
			PlayMoveTask();
		}
		else
		{
			PerceptualObjectSubsystem->PerceptualObjectLocatorChangeEvent.AddDynamic(this, &UPieceAction_Move::ReceivePerceptualObjectLocatorChangeEventCallback);
		}
	}
}

void UPieceAction_Move::PlayMoveTask()
{
	MoveTask = UAnimTask_Move::CreateTask(this, StartLocator, EndLocator, Data->AnimTaskMoveData);
	if (!MoveTask->GetIsActiving())
	{
		Finish();
		return;
	}

	UCookieLandPieceLibrary::AddActiveGameplayTag_Moving(Piece);

	MoveTask->FinishEvent.AddDynamic(this, &UPieceAction_Move::MoveTaskFinishEventCallback);

	DestroyCueActor(this, CueActor_Move);

	UCookieLandCueData_Move* CueData = NewObject<UCookieLandCueData_Move>();
	CueData->MoveOrientation = Data->MoveOrientation;
	CueActor_Move = CreateCueActor(this, GET_GAMEPLAY_TAG("CueActor.Action.Move"), CueData);
}

void UPieceAction_Move::ReceivePerceptualObjectLocatorChangeEventCallback(int InId, FCookieLandPieceLocator OldLocator, FCookieLandPieceLocator NewLocator)
{
	if (NewLocator == StartLocator) 
	{
		UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
		PerceptualObjectSubsystem->PerceptualObjectLocatorChangeEvent.RemoveDynamic(this, &UPieceAction_Move::ReceivePerceptualObjectLocatorChangeEventCallback);

		PlayMoveTask();
	}
}

void UPieceAction_Move::MoveTaskFinishEventCallback()
{
	Finish();
}


void UPieceAction_Move::ReceiveMapLocatorOccupyStateChangeEventCallback(FCookieLandPieceLocator Locator, bool Occupy)
{
	EndLocator = GetMoveTarget();

	if (StartLocator != EndLocator)
	{
		UCookieLandMapBuilder* MapBuilder = GetMapBuilder();
		if (MapBuilder)
		{
			MapBuilder->MapLocatorOccupyStateChangeEvent.RemoveDynamic(this, &UPieceAction_Move::ReceiveMapLocatorOccupyStateChangeEventCallback);
		}
		TryTrigger(Data->Trigger);
	}
}

#pragma endregion

#pragma region UPieceAction_MoveBackAndForth

UPieceActionData_MoveBackAndForth* UPieceAction_MoveBackAndForth::GetMyData()
{
	return Cast<UPieceActionData_MoveBackAndForth>(BaseData);
}

void UPieceAction_MoveBackAndForth::MoveTaskFinishEventCallback()
{
	StartLocator = EndLocator;
	MoveOrientation = UCookieLandMapBuildLibrary::GetOppositeOrientation(MoveOrientation);
	EndLocator = GetMoveTarget();

	if (StartLocator == EndLocator)
	{
		UCookieLandMapBuilder* MapBuilder = GetMapBuilder();
		if (MapBuilder)
		{
			MapBuilder->MapLocatorOccupyStateChangeEvent.AddDynamic(this, &UPieceAction_Move::ReceiveMapLocatorOccupyStateChangeEventCallback);
		}
		else
		{
			Finish();
		}
		return;
	}

	UPieceActionData_MoveBackAndForth* MyData = GetMyData();
	TryTrigger(MyData->AgainTrigger);
}
#pragma endregion