// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapShowDirector.h"
#include "CookieLand/Map/Public/CookieLandMapBuildActor.h"
#include "CookieLand/Map/Public/CookieLandMapBuilder.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"
#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectSubsystem.h"


void UCookieLandMapShowDirector::SetBuildActor(ACookieLandMapBuildActor* InBuildActor)
{
	BuildActor = InBuildActor;
}

void UCookieLandMapShowDirector::SetDisplayAll(bool bValue)
{
	bDisplayAll = bValue;
}

bool UCookieLandMapShowDirector::GetEnableDisplay(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (bDisplayAll)
	{
		return true;
	}

	if (!BuildActor || !BuildActor->GetMapBuilder())
	{
		return false;
	}
	
	//todo:后续要考虑即便当前不存在地块，但是基于立方体的表现需要显示
	if (!BuildActor->GetMapBuilder()->GetIsPieceOccupyByLocation(MapCubeLocation, PieceOrientation))
	{
		return false;
	}
	
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem(this);
	if (!PerceptualObjectSubsystem) 
	{
		return false;
	}

	FCookieLandPieceLocator MainLocator;
	if (!PerceptualObjectSubsystem->GetMainCurrentLocator(MainLocator))
	{
		return false;
	}

	// 主角色当前站立的必然显示
	if (MainLocator.PieceLocation == MapCubeLocation)
	{
		return true;
	}

	// 显示类型
	if(!GetEnableDisplaySwitchMapShowType(MainLocator, PieceOrientation))
	if (!PerceptualObjectSubsystem->GetMainCurrentLocator(MainLocator))
	{
		return false;
	}

	// 层数
	if (MapShowInfo.bConfineFloor)
	{
		if (abs(MainLocator.PieceLocation.Floor - MapCubeLocation.Floor) > MapShowInfo.ConfineFloorNumber)
		{
			return false;
		}
	}

	// 周围
	if (MapShowInfo.bConfineRound)
	{
		if (abs(MainLocator.PieceLocation.X - MapCubeLocation.X) > MapShowInfo.ConfineRoundNumber || (abs(MainLocator.PieceLocation.Y - MapCubeLocation.Y) > MapShowInfo.ConfineRoundNumber))
		{
			return false;
		}
	}
	 
	// 立方体遮蔽
	TMap<ECookieLandPieceOrientation, int> RelativeOrientationAndDistances = MapCubeLocation.GetRelativeOrientationAndDistances(MainLocator.PieceLocation);
	for(TMap<ECookieLandPieceOrientation, int>::TIterator iter = RelativeOrientationAndDistances.CreateIterator();iter;++iter)
	{
		ECookieLandPieceOrientation RelativeOrientation = iter.Key();
		int Distance = iter.Value();

		FCookieLandOrientationLinkInfo ForceLinkInfo;
		if (BuildActor->GetMapBuilder()->GetNearestForceLineInfo(RelativeOrientation, MapCubeLocation, Distance, ForceLinkInfo))
		{
			// 立方体遮挡
			if (MapShowInfo.bCubeOcclusionDisplay)
			{
				// 内部
				if (BuildActor->GetMapBuilder()->GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(ForceLinkInfo, MapCubeLocation))
				{
					if (MapShowInfo.bOnlyDisplayEgdeWhenCubeOcclusionDisplay)
					{
						return false;
					}
				}
				// 不在连接中
				else if (!BuildActor->GetMapBuilder()->GetInEdgeByLineInfo(ForceLinkInfo, MapCubeLocation))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool UCookieLandMapShowDirector::GetEnableDisplaySwitchMapShowType(FCookieLandPieceLocator MainLocator, const ECookieLandPieceOrientation PieceOrientation)
{
	switch (MapShowInfo.MapShowType)
	{
	case ECookieLandMapShowType::Plane:
		return PieceOrientation == MainLocator.PieceOrientation;
	case ECookieLandMapShowType::ThreeDimensions:
		UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem(this);
		if (PerceptualObjectSubsystem)
		{
			ECookieLandMapAngleViewType MapAngleViewType = PerceptualObjectSubsystem->GetMapAngleViewType();
			TArray<ECookieLandPieceOrientation> Orientations = UCookieLandMapBuildLibrary::GetMapAgnleViewOrientations(MapAngleViewType, PieceOrientation);
			for (int Index = 0; Index < Orientations.Num(); ++Index)
			{
				if (PieceOrientation == Orientations[Index])
				{
					return true;
				}
			}
		}
		return false;
	}
	return false;
}