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

	UpdateMapRange(true, MapCubeLocation, PieceOrientation);

	return true;
}

bool UCookieLandMapBuilder::ReleasePieceByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (!GetIsPieceOccupyByLocation(MapCubeLocation, PieceOrientation))
	{
		return false;
	}

	UpdateMapRange(false, MapCubeLocation, PieceOrientation);

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


bool UCookieLandMapBuilder::PieceForceLine(const FCookieLandLocation RequsetPieceLocation, const FCookieLandLocation TryLinePieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (!GetIsPieceOccupyByLocation(RequsetPieceLocation, PieceOrientation) || !GetIsPieceOccupyByLocation(TryLinePieceLocation, PieceOrientation) || TryLinePieceLocation == RequsetPieceLocation)
	{
		return false;
	}

	UCookieLandMapRangeInfo* MapRangeInfo = CreateOrGetMapRangeInfo(RequsetPieceLocation, PieceOrientation);

	// 同方向的才能执行连接
	if (!MapRangeInfo->GetIsConformanceRange(TryLinePieceLocation))
	{
		return false;
	}

	FCookieLandLocation MinPieceLocation, MaxPieceLocation;
	if (RequsetPieceLocation.GetIsMaxByOrientation(PieceOrientation, TryLinePieceLocation))
	{
		MaxPieceLocation = RequsetPieceLocation;
		MinPieceLocation = TryLinePieceLocation;
	}
	else
	{
		MaxPieceLocation = TryLinePieceLocation;
		MinPieceLocation = RequsetPieceLocation;
	}

	FCookieLandOrientationLinkInfo MinLineInfo;
	GetForceLineInfo(MinPieceLocation, PieceOrientation, MinLineInfo);

	FCookieLandOrientationLinkInfo MaxLineInfo;
	GetForceLineInfo(MaxPieceLocation, PieceOrientation, MaxLineInfo);

	// 判断是否都在连接中
	if (MinLineInfo.GetIsValid() && MaxLineInfo.GetIsValid())
	{
		// 判断是否在同一个连接中
		if (MinLineInfo.Max_PieceLocation == MaxLineInfo.Max_PieceLocation && MinLineInfo.Min_PieceLocation == MaxLineInfo.Min_PieceLocation)
		{
			return false;
		}

		// 判断是否内部状态且非边缘，无法发起连接
		if (GetsWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(MinLineInfo, MinPieceLocation) || GetsWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(MaxLineInfo, MaxPieceLocation))
		{
			return false;
		}

		// 两个模块连接,中间的模块也会被合并
		TArray< FCookieLandOrientationLinkInfo> DeleteLineInfos;
		for (int Index = 0; Index < MapRangeInfo->ForceLineInfos.Num(); ++Index)
		{
			FCookieLandOrientationLinkInfo LineInfo = MapRangeInfo->ForceLineInfos[Index];
			if (LineInfo.Min_PieceLocation.GetIsMaxByOrientation(PieceOrientation, MinLineInfo.Max_PieceLocation) && LineInfo.Max_PieceLocation.GetIsMinByOrientation(PieceOrientation, MaxLineInfo.Min_PieceLocation))
			{
				DeleteLineInfos.Add(LineInfo);
			}
		}
		DeleteLineInfos.Add(MinLineInfo);
		DeleteLineInfos.Add(MaxLineInfo);

		//清理模块
		for (int Index = 0; Index < DeleteLineInfos.Num(); ++Index)
		{
			FCookieLandOrientationLinkInfo DeleteLineInfo = DeleteLineInfos[Index];
			MapRangeInfo->ForceLineInfos.Remove(DeleteLineInfo);
		}

		// 得到新模块
		MinLineInfo.SetMax(MaxLineInfo.Max_PieceLocation);
		MapRangeInfo->ForceLineInfos.Add(MinLineInfo);
	}

	// 其中一个在连接中
	else if(MinLineInfo.GetIsValid() || MinLineInfo.GetIsValid())
	{
		FCookieLandOrientationLinkInfo* LineInfoPtr = MinLineInfo.GetIsValid() ? &MinLineInfo : &MinLineInfo;
		FCookieLandLocation* AddLocation = MinLineInfo.GetIsValid() ? &MaxPieceLocation : &MinPieceLocation;

		// 判断是否内部状态且非边缘，无法发起连接
		if (GetsWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(*LineInfoPtr, *AddLocation))
		{
			return false;
		}

		// 添加地块
		LineInfoPtr->AddLocation(*AddLocation);

		// 记录下来
		MapRangeInfo->ForceLineInfos.Remove(*LineInfoPtr);
		MapRangeInfo->ForceLineInfos.Add(*LineInfoPtr);
	}
	
	// 没有任何连接
	else
	{
		FCookieLandOrientationLinkInfo LineInfo;
		LineInfo.SetData(PieceOrientation, MaxPieceLocation, MinPieceLocation);

		MapRangeInfo->ForceLineInfos.Add(LineInfo);
	}

	return true;
}

bool UCookieLandMapBuilder::PieceDeleteForceLine(const FCookieLandLocation RequsetPieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (!GetIsPieceOccupyByLocation(RequsetPieceLocation, PieceOrientation))
	{
		return false;
	}

	FCookieLandOrientationLinkInfo LinkRangeInfo;

	// 并非强制连接,无需解绑
	if (!GetForceLineInfo(RequsetPieceLocation, PieceOrientation, LinkRangeInfo))
	{
		return false;
	}

	if (!LinkRangeInfo.GetIsValid())
	{
		return false;
	}

	// 判断是否内部状态且非边缘，无法解绑连接
	if (GetsWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(LinkRangeInfo,RequsetPieceLocation))
	{
		return false;
	}

	// 移除绑定
	UCookieLandMapRangeInfo* MapRangeInfo = CreateOrGetMapRangeInfo(RequsetPieceLocation, PieceOrientation);
	MapRangeInfo->ForceLineInfos.Remove(LinkRangeInfo);

	return true;
}

bool UCookieLandMapBuilder::GetsWhetherInternalStateOfForcedLineAndNotAtEdge(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	FCookieLandOrientationLinkInfo LineInfo;
	if (GetForceLineInfo(PieceLocation, PieceOrientation, LineInfo))
	{
		return GetsWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(LineInfo, PieceLocation);
	}
	return false;
}

bool UCookieLandMapBuilder::GetsWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(const FCookieLandOrientationLinkInfo& LineInfo, const FCookieLandLocation PieceLocation)
{
	if (LineInfo.GetIsValid())
	{
		if (PieceLocation.GetIsMaxByOrientation(LineInfo.Orientation, LineInfo.Min_PieceLocation) && PieceLocation.GetIsMinByOrientation(LineInfo.Orientation, LineInfo.Max_PieceLocation))
		{
			return true;
		}
	}

	return false;
}

bool UCookieLandMapBuilder::GetForceLineInfo(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation, FCookieLandOrientationLinkInfo& OutLineInfo)
{
	UCookieLandMapRangeInfo* MapRangeInfo = CreateOrGetMapRangeInfo(PieceLocation, PieceOrientation);
	if (MapRangeInfo->ForceLineInfos.Num() == 0)
	{
		return false;
	}

	for (int Index = 0; Index < MapRangeInfo->ForceLineInfos.Num(); ++Index)
	{
		FCookieLandOrientationLinkInfo LinkInfo = MapRangeInfo->ForceLineInfos[Index];
		if (LinkInfo.GetIsInSide(PieceLocation))
		{
			OutLineInfo = LinkInfo;
			return true;
		}
	}

	return false;
}

bool UCookieLandMapRangeInfo::GetIsConformanceRange(const FCookieLandLocation MapCubeLocation)
{
	switch(Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		if (Orientation == ECookieLandPieceOrientation::Up)
		{
			return Location.X == MapCubeLocation.X && Location.Y == MapCubeLocation.Y;
		}
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		if (Orientation == ECookieLandPieceOrientation::Left)
		{
			return Location.Y == MapCubeLocation.Y && Location.Floor == MapCubeLocation.Floor;
		}
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		if (Orientation == ECookieLandPieceOrientation::Forward)
		{
			return Location.X == MapCubeLocation.X && Location.Floor == MapCubeLocation.Floor;
		}
		break;
	};

	return false;
}

void FCookieLandOrientationLinkInfo::SetData(ECookieLandPieceOrientation InOrientation, FCookieLandLocation InMax, FCookieLandLocation InMin)
{
	bValid = true;
	Max_PieceLocation = InMax;
	Min_PieceLocation = InMin;
	Orientation = InOrientation;	
	UpdateDistance();
}

void FCookieLandOrientationLinkInfo::SetMax(FCookieLandLocation InMax)
{
	Max_PieceLocation = InMax;
	UpdateDistance();
}

void FCookieLandOrientationLinkInfo::SetMin(FCookieLandLocation InMin)
{
	Min_PieceLocation = InMin;
	UpdateDistance();
}

void FCookieLandOrientationLinkInfo::AddLocation(FCookieLandLocation Location)
{
	if (Location.GetIsMinByOrientation(Orientation, Min_PieceLocation))
	{
		SetMin(Location);
	}
	else if (Location.GetIsMaxByOrientation(Orientation, Max_PieceLocation))
	{
		SetMax(Location);
	}
}

void FCookieLandOrientationLinkInfo::UpdateDistance()
{
	Distance = 0;

	switch (Orientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		Distance = Max_PieceLocation.Floor - Min_PieceLocation.Floor;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		Distance = Max_PieceLocation.X - Min_PieceLocation.X;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		Distance = Max_PieceLocation.Y - Min_PieceLocation.Y;
		break;
	};
}

void FCookieLandOrientationLinkInfo::ClearData()
{
	Max_PieceLocation = FCookieLandLocation();
	Min_PieceLocation = FCookieLandLocation();
	Distance = 0;
	bValid = false;
}

bool FCookieLandOrientationLinkInfo::GetIsInSide(FCookieLandLocation Location)
{
	if (Location.GetIsMaxEqualByOrientation(Orientation, Min_PieceLocation) && Location.GetIsMinEqualByOrientation(Orientation, Max_PieceLocation))
	{
		return true;
	}
	return false;
}

void UCookieLandMapRangeInfo::AddPiece(const FCookieLandLocation PieceLocation)
{
	PieceLocastions.Add(PieceLocation);
}

void UCookieLandMapRangeInfo::RemovePiece(const FCookieLandLocation PieceLocation)
{
	PieceLocastions.Remove(PieceLocation);
}

UCookieLandMapRangeInfo* UCookieLandMapBuilder::CreateOrGetMapRangeInfo(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	for (int Index = 0; Index < MapRangeInfos.Num(); ++Index)
	{
		UCookieLandMapRangeInfo* MapRangeInfo = MapRangeInfos[Index];
		if (MapRangeInfo->GetIsConformanceRange(PieceLocation))
		{
			return MapRangeInfo;
		}
	}

	UCookieLandMapRangeInfo* MapRangeInfo = NewObject<UCookieLandMapRangeInfo>();
	switch (PieceOrientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		MapRangeInfo->Orientation = ECookieLandPieceOrientation::Up;
		MapRangeInfo->Location.X = PieceLocation.X;
		MapRangeInfo->Location.Y = PieceLocation.Y;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		MapRangeInfo->Orientation = ECookieLandPieceOrientation::Left;
		MapRangeInfo->Location.Y = PieceLocation.Y;
		MapRangeInfo->Location.Floor = PieceLocation.Floor;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		MapRangeInfo->Orientation = ECookieLandPieceOrientation::Forward;
		MapRangeInfo->Location.X = PieceLocation.X;
		MapRangeInfo->Location.Floor = PieceLocation.Floor;
		break;
	};

	return MapRangeInfo;
}

void UCookieLandMapBuilder::UpdateMapRange(bool bIsAdd, const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandMapRangeInfo* MapRangeInfo = CreateOrGetMapRangeInfo(PieceLocation, PieceOrientation);

	if (bIsAdd)
	{
		MapRangeInfo->AddPiece(PieceLocation);
	}
	else
	{
		// 地块被移除时，自动移除强制绑定
		PieceDeleteForceLine(PieceLocation, PieceOrientation);
		MapRangeInfo->RemovePiece(PieceLocation);
	}
}