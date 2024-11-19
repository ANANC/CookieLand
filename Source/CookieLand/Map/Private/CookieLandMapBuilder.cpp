// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Public/CookieLandMapBuilder.h"


void UCookieLandMapCubeInfo::OccupyPiece(const ECookieLandPieceOrientation PieceOrientation)
{
	if (OrientationOccupys.Contains(PieceOrientation))
	{
		OrientationOccupys[PieceOrientation] = true;
	}
	else
	{
		OrientationOccupys.Add(PieceOrientation, true);
	}

	UpdateOccupyState();
}

void UCookieLandMapCubeInfo::ReleasePiece(const ECookieLandPieceOrientation PieceOrientation)
{
	if (OrientationOccupys.Contains(PieceOrientation))
	{
		OrientationOccupys.Remove(PieceOrientation);
	}

	UpdateOccupyState();
}

void UCookieLandMapCubeInfo::UpdateOccupyState()
{
	bool bNewOccupy = false;
	for (TMap<ECookieLandPieceOrientation, bool>::TIterator iter = OrientationOccupys.CreateIterator(); iter; ++iter)
	{
		bool bPieceOccupy = iter.Value();
		if (bPieceOccupy)
		{
			bNewOccupy = true;
			break;
		}
	}

	bOccupy = bNewOccupy;
}

void UCookieLandMapCubeInfo::AddForceLine(const ECookieLandPieceOrientation PieceOrientation, const FCookieLandLocation LineLocation)
{
	if (OrientationForceLinks.Contains(PieceOrientation))
	{
		OrientationForceLinks[PieceOrientation] = LineLocation;
	}
	else
	{
		OrientationForceLinks.Add(PieceOrientation, LineLocation);
	}

	UpdateForceLineState();
}

void UCookieLandMapCubeInfo::DeleteForceLine(const ECookieLandPieceOrientation PieceOrientation)
{
	if (OrientationForceLinks.Contains(PieceOrientation))
	{
		OrientationForceLinks.Remove(PieceOrientation);
	}

	UpdateForceLineState();
}

void UCookieLandMapCubeInfo::UpdateForceLineState()
{
	bool bNewForceLine = OrientationForceLinks.Num() > 0;
	bForceLine = bNewForceLine;
}

void UCookieLandMapBuilder::Init()
{

}

void UCookieLandMapBuilder::UnInit()
{
	MapCubeInfos.Empty();
	Location2MapCubeInfos.Empty();
}

bool UCookieLandMapBuilder::GetMapIsEmpty()
{
	bool bIsEmpty = true;
	for (int Index = 0; Index < MapCubeInfos.Num(); ++Index)
	{
		UCookieLandMapCubeInfo* MapCubeInfo = MapCubeInfos[Index];
		if (MapCubeInfo->bOccupy)
		{
			bIsEmpty = false;
			break;
		}
	}

	return bIsEmpty;
}

bool UCookieLandMapBuilder::GetIsCubeOccupyByLocation(const FCookieLandLocation MapCubeLocation)
{
	bool bOccupy = false;
	UCookieLandMapCubeInfo* MapCubeInfo = GetMapCubeInfo(MapCubeLocation);
	if (MapCubeInfo)
	{
		bOccupy = MapCubeInfo->bOccupy;
	}

	return bOccupy;
}

bool UCookieLandMapBuilder::GetIsPieceOccupyByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	bool bOccupy = false;
	UCookieLandMapCubeInfo* MapCubeInfo = GetMapCubeInfo(MapCubeLocation);
	if (MapCubeInfo)
	{
		bool* bPieceOrientationOccupy = MapCubeInfo->OrientationOccupys.Find(PieceOrientation);
		if (bPieceOrientationOccupy)
		{
			bOccupy = *bPieceOrientationOccupy;
		}
	}

	return bOccupy;
}

bool UCookieLandMapBuilder::OccupyPieceByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (GetIsPieceOccupyByLocation(MapCubeLocation, PieceOrientation))
	{
		return false;
	}

	UCookieLandMapCubeInfo* MapCubeInfo = CreateOrGetMapCubeInfo(MapCubeLocation);
	MapCubeInfo->OccupyPiece(PieceOrientation);

	return true;
}

bool UCookieLandMapBuilder::ReleasePieceByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (!GetIsPieceOccupyByLocation(MapCubeLocation, PieceOrientation))
	{
		return false;
	}

	UCookieLandMapCubeInfo* MapCubeInfo = CreateOrGetMapCubeInfo(MapCubeLocation);
	MapCubeInfo->ReleasePiece(PieceOrientation);

	return true;
}

TArray< FCookieLandPieceLocator> UCookieLandMapBuilder::GetAllOccupyPiece()
{
	TArray< FCookieLandPieceLocator> PieceLocators;

	for (int Index = 0; Index < MapCubeInfos.Num(); ++Index)
	{
		UCookieLandMapCubeInfo* MapCubeInfo = MapCubeInfos[Index];
		if (!MapCubeInfo->bOccupy)
		{
			continue;
		}

		for (TMap<ECookieLandPieceOrientation, bool>::TIterator iter = MapCubeInfo->OrientationOccupys.CreateIterator(); iter; ++iter)
		{
			bool bPieceOccupy = iter.Value();
			if (bPieceOccupy)
			{
				ECookieLandPieceOrientation PieceOrientation = iter.Key();

				FCookieLandPieceLocator PieceLocator(MapCubeInfo->Location, PieceOrientation);
				PieceLocators.Add(PieceLocator);
			}
		}
	}

	return PieceLocators;
}

TArray< FCookieLandPieceLocator> UCookieLandMapBuilder::GetAllOccupyPieceByTargetFloor(int InFloor)
{
	TArray< FCookieLandPieceLocator> PieceLocators;

	for (int Index = 0; Index < MapCubeInfos.Num(); ++Index)
	{
		UCookieLandMapCubeInfo* MapCubeInfo = MapCubeInfos[Index];
		if (!MapCubeInfo->bOccupy || MapCubeInfo->Location.Floor != InFloor)
		{
			continue;
		}

		for (TMap<ECookieLandPieceOrientation, bool>::TIterator iter = MapCubeInfo->OrientationOccupys.CreateIterator(); iter; ++iter)
		{
			bool bPieceOccupy = iter.Value();
			if (bPieceOccupy)
			{
				ECookieLandPieceOrientation PieceOrientation = iter.Key();

				FCookieLandPieceLocator PieceLocator(MapCubeInfo->Location, PieceOrientation);
				PieceLocators.Add(PieceLocator);
			}
		}
	}

	return PieceLocators;
}

TArray<int> UCookieLandMapBuilder::GetAllOccupyFloor()
{
	TArray<int> OccupyFloors;

	for (int Index = 0; Index < MapCubeInfos.Num(); ++Index)
	{
		UCookieLandMapCubeInfo* MapCubeInfo = MapCubeInfos[Index];
		if (!MapCubeInfo->bOccupy)
		{
			continue;
		}

		OccupyFloors.Add(MapCubeInfo->Location.Floor);
	}

	return OccupyFloors;
}

UCookieLandMapCubeInfo* UCookieLandMapBuilder::CreateOrGetMapCubeInfo(const FCookieLandLocation MapCubeLocation)
{
	UCookieLandMapCubeInfo* MapCubeInfo = GetMapCubeInfo(MapCubeLocation);
	if (MapCubeInfo)
	{
		return MapCubeInfo;
	}

	MapCubeInfo = NewObject<UCookieLandMapCubeInfo>();
	MapCubeInfo->Location = MapCubeLocation;

	Location2MapCubeInfos.Add(MapCubeLocation.ToVector(), MapCubeInfo);
	MapCubeInfos.Add(MapCubeInfo);

	return MapCubeInfo;
}

UCookieLandMapCubeInfo* UCookieLandMapBuilder::GetMapCubeInfo(const FCookieLandLocation MapCubeLocation)
{
	UCookieLandMapCubeInfo** MapCubeInfoPtr = Location2MapCubeInfos.Find(MapCubeLocation.ToVector());
	if (MapCubeInfoPtr)
	{
		return *MapCubeInfoPtr;
	}
	return nullptr;
}


bool UCookieLandMapBuilder::GetPieceForceLineLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation, FCookieLandPieceLocator& OutLineLocator)
{
	UCookieLandMapCubeInfo* MapCubeInfo = GetMapCubeInfo(MapCubeLocation);
	if (!MapCubeInfo)
	{
		return false;
	}

	if (MapCubeInfo->OrientationForceLinks.Contains(PieceOrientation))
	{
		OutLineLocator.PieceLocation = MapCubeInfo->OrientationForceLinks[PieceOrientation];
		OutLineLocator.PieceOrientation = PieceOrientation;

		return true;
	}

	return false;
}

bool UCookieLandMapBuilder::PieceForceLine(const FCookieLandLocation RequsetPieceLocation, const FCookieLandLocation TryLinePieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (!GetIsPieceOccupyByLocation(RequsetPieceLocation, PieceOrientation) || !GetIsPieceOccupyByLocation(TryLinePieceLocation, PieceOrientation))
	{
		return false;
	}

	switch (PieceOrientation)
	{
	case ECookieLandPieceOrientation::Up:
		if (RequsetPieceLocation.X != TryLinePieceLocation.X || RequsetPieceLocation.Y != TryLinePieceLocation.Y)
		{
			return false;
		}
		break;
	case ECookieLandPieceOrientation::Down:
		if (RequsetPieceLocation.X != TryLinePieceLocation.X || RequsetPieceLocation.Y != TryLinePieceLocation.Y)
		{
			return false;
		}
		break;
	case ECookieLandPieceOrientation::Left:
		if (RequsetPieceLocation.Y != TryLinePieceLocation.Y || RequsetPieceLocation.Floor != TryLinePieceLocation.Floor)
		{
			return false;
		}
		break;
	case ECookieLandPieceOrientation::Right:
		if (RequsetPieceLocation.Y != TryLinePieceLocation.Y || RequsetPieceLocation.Floor != TryLinePieceLocation.Floor)
		{
			return false;
		}
		break;
		break;
	case ECookieLandPieceOrientation::Forward:
		if (RequsetPieceLocation.X != TryLinePieceLocation.X || RequsetPieceLocation.Floor != TryLinePieceLocation.Floor)
		{
			return false;
		}
		break;
		break;
	case ECookieLandPieceOrientation::Backward:
		if (RequsetPieceLocation.X != TryLinePieceLocation.X || RequsetPieceLocation.Floor != TryLinePieceLocation.Floor)
		{
			return false;
		}
		break;
	};

	UCookieLandMapCubeInfo* RequsetMapCubeInfo = GetMapCubeInfo(RequsetPieceLocation);
	if (!RequsetMapCubeInfo)
	{
		return false;
	}
	//todo:要判断是否内部状态且非边缘，无法发起连接

	//todo:连接要判断自动连接状态

	if (RequsetMapCubeInfo->OrientationForceLinks.Contains(PieceOrientation))
	{
		RequsetMapCubeInfo->OrientationForceLinks.Remove(PieceOrientation);
	}

	RequsetMapCubeInfo->OrientationForceLinks.Add(PieceOrientation, RequsetPieceLocation);

	return true;
}

bool UCookieLandMapBuilder::GetLineRangeInfo(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation, FCookieLandOrientationLinkRangeInfo& OutLineRangeInfo)
{
	return false;
}