// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Action/PieceAction_Move.h"
#include "CookieLand/Map/CookieLandPiece.h"
#include "CookieLand/Map/CookieLandMapBuildActor.h"
#include "CookieLand/Map/CookieLandMapBuilder.h"

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

	if (StartLocator == EndLocator)
	{
		return;
	}


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
			if (MapBuilder->GetNearestPieceLocator(StartLocator, Data->MoveOrientation, Locator))
			{
				Locator = NearestLocator;
			}
		}
	}
	else if (Data->MoveType == EPieceActionMoveType::Fixed)
	{
		Locator.PieceLocation.AddDistanceBySixDirection(Data->MoveOrientation, Data->FixedDistance);
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