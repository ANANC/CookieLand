// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapBuilder.h"


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

bool UCookieLandMapBuilder::GetIsPieceOccupyByLocator(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
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
	if (GetIsPieceOccupyByLocator(MapCubeLocation, PieceOrientation))
	{
		return false;
	}

	UCookieLandMapCubeInfo* MapCubeInfo = CreateOrGetMapCubeInfo(MapCubeLocation);
	MapCubeInfo->OccupyPiece(PieceOrientation);

	UpdateMapRange(true, MapCubeLocation, PieceOrientation);

	if (MapLocatorOccupyStateChangeEvent.IsBound())
	{
		MapLocatorOccupyStateChangeEvent.Broadcast(FCookieLandPieceLocator(MapCubeLocation, PieceOrientation),true);
	}

	return true;
}

bool UCookieLandMapBuilder::ReleasePieceByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (!GetIsPieceOccupyByLocator(MapCubeLocation, PieceOrientation))
	{
		return false;
	}

	UpdateMapRange(false, MapCubeLocation, PieceOrientation);

	UCookieLandMapCubeInfo* MapCubeInfo = CreateOrGetMapCubeInfo(MapCubeLocation);
	MapCubeInfo->ReleasePiece(PieceOrientation);

	if (MapLocatorOccupyStateChangeEvent.IsBound())
	{
		MapLocatorOccupyStateChangeEvent.Broadcast(FCookieLandPieceLocator(MapCubeLocation, PieceOrientation), false);
	}

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

bool UCookieLandMapBuilder::GetNearestPieceLocator(const FCookieLandPieceLocator SourceLocator, ECookieLandPieceOrientation Orientation, FCookieLandPieceLocator& OutNearestLocator)
{
	UCookieLandMapRangeInfo* MapRangeInfo = nullptr;

	TArray<UCookieLandMapRangeInfo*> OutMapOmnibearingRangeInfos = CreateOrGetMapOmnibearingRangeInfos(SourceLocator.PieceLocation);
	for (int Index = 0; Index < OutMapOmnibearingRangeInfos.Num(); ++Index)
	{
		UCookieLandMapRangeInfo* OutMapOmnibearingRangeInfo = OutMapOmnibearingRangeInfos[Index];
		if (!OutMapOmnibearingRangeInfo->GetIsConformanceRange(SourceLocator.PieceLocation, Orientation))
		{
			continue;
		}

		MapRangeInfo = OutMapOmnibearingRangeInfo;
		break;
	}

	if (!MapRangeInfo)
	{
		return false;
	}

	FCookieLandPieceLocator* NearestLocator = nullptr;
	int MinDistance = -1;
	for (int Index = 0; Index < MapRangeInfo->PieceLocastions.Num(); ++Index)
	{
		FCookieLandPieceLocator Locator(MapRangeInfo->PieceLocastions[Index], SourceLocator.PieceOrientation);
		if (!GetIsPieceOccupyByLocator(Locator.PieceLocation, Locator.PieceOrientation) || Locator == SourceLocator)
		{
			continue;
		}

		TMap<ECookieLandPieceOrientation, int> RelativeOrientationAndDistances = SourceLocator.PieceLocation.GetRelativeOrientationAndDistances(Locator.PieceLocation);
		if (RelativeOrientationAndDistances.Contains(Orientation))
		{
			int Distance = RelativeOrientationAndDistances[Orientation];
			if (MinDistance == -1 || MinDistance > Distance)
			{
				MinDistance = Distance;
				NearestLocator = &Locator;
			}
		}
	}

	if (NearestLocator)
	{
		OutNearestLocator = *NearestLocator;
		return true;
	}
	return false;
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
	if (!GetIsPieceOccupyByLocator(RequsetPieceLocation, PieceOrientation) || !GetIsPieceOccupyByLocator(TryLinePieceLocation, PieceOrientation) || TryLinePieceLocation == RequsetPieceLocation)
	{
		return false;
	}

	UCookieLandMapRangeInfo* MapRangeInfo = CreateOrGetMapRangeInfo(RequsetPieceLocation, PieceOrientation);

	// 同方向的才能执行连接
	if (!MapRangeInfo->GetIsConformanceRange(TryLinePieceLocation, PieceOrientation))
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
		if (GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(MinLineInfo, MinPieceLocation) || GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(MaxLineInfo, MaxPieceLocation))
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
	else if(MinLineInfo.GetIsValid() || MaxLineInfo.GetIsValid())
	{
		FCookieLandOrientationLinkInfo* LineInfoPtr = MinLineInfo.GetIsValid() ? &MinLineInfo : &MaxLineInfo;
		FCookieLandLocation* AddLocation = MinLineInfo.GetIsValid() ? &MaxPieceLocation : &MinPieceLocation;

		// 判断是否内部状态且非边缘，无法发起连接
		if (GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(*LineInfoPtr, *AddLocation))
		{
			return false;
		}
		MapRangeInfo->ForceLineInfos.Remove(*LineInfoPtr);

		// 添加地块
		LineInfoPtr->AddLocation(*AddLocation);

		// 记录下来
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
	if (!GetIsPieceOccupyByLocator(RequsetPieceLocation, PieceOrientation))
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
	if (GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(LinkRangeInfo,RequsetPieceLocation))
	{
		return false;
	}

	// 移除绑定
	UCookieLandMapRangeInfo* MapRangeInfo = CreateOrGetMapRangeInfo(RequsetPieceLocation, PieceOrientation);
	MapRangeInfo->ForceLineInfos.Remove(LinkRangeInfo);

	return true;
}

bool UCookieLandMapBuilder::GetWhetherInternalStateOfForcedLineAndNotAtEdge(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	FCookieLandOrientationLinkInfo LineInfo;
	if (GetForceLineInfo(PieceLocation, PieceOrientation, LineInfo))
	{
		return GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(LineInfo, PieceLocation);
	}
	return false;
}

bool UCookieLandMapBuilder::GetWhetherInternalStateOfForcedLineAndNotAtEdgeByLineInfo(const FCookieLandOrientationLinkInfo& LineInfo, const FCookieLandLocation PieceLocation)
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

bool UCookieLandMapBuilder::GetInEdgeByLineInfo(const FCookieLandOrientationLinkInfo& LineInfo, const FCookieLandLocation PieceLocation)
{
	if (LineInfo.GetIsValid())
	{
		if (PieceLocation == LineInfo.Min_PieceLocation || PieceLocation == LineInfo.Max_PieceLocation)
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

bool UCookieLandMapBuilder::GetNearestForceLineInfo(const ECookieLandPieceOrientation PieceOrientation, const FCookieLandLocation StartPieceLocation, int Distance, FCookieLandOrientationLinkInfo& OutLineInfo)
{
	UCookieLandMapRangeInfo* MapRangeInfo = CreateOrGetMapRangeInfo(StartPieceLocation, PieceOrientation);
	if (MapRangeInfo->ForceLineInfos.Num() == 0)
	{
		return false;
	}

	for (int Index = 0; Index < Distance; ++Index)
	{
		FCookieLandLocation FindLocation(StartPieceLocation);
		FindLocation.AddDistanceBySixDirection(PieceOrientation, Distance);

		if (GetForceLineInfo(FindLocation, PieceOrientation, OutLineInfo))
		{
			return true;
		}
	}
	return false;
}

void UCookieLandMapRangeInfo::Init(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	switch (PieceOrientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		Orientation = ECookieLandPieceOrientation::Up;
		Location.X = PieceLocation.X;
		Location.Y = PieceLocation.Y;
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		Orientation = ECookieLandPieceOrientation::Left;
		Location.Y = PieceLocation.Y;
		Location.Floor = PieceLocation.Floor;
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		Orientation = ECookieLandPieceOrientation::Forward;
		Location.X = PieceLocation.X;
		Location.Floor = PieceLocation.Floor;
		break;
	};
}

bool UCookieLandMapRangeInfo::GetIsConformanceRange(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	switch(PieceOrientation)
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

void UCookieLandMapRangeInfo::AddPiece(const FCookieLandLocation PieceLocation)
{
	PieceLocastions.AddUnique(PieceLocation);
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
		if (MapRangeInfo->GetIsConformanceRange(PieceLocation, PieceOrientation))
		{
			return MapRangeInfo;
		}
	}

	UCookieLandMapRangeInfo* MapRangeInfo = NewObject<UCookieLandMapRangeInfo>();
	MapRangeInfo->Init(PieceLocation, PieceOrientation);

	MapRangeInfos.Add(MapRangeInfo);

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
		MapRangeInfoTryRemove(MapRangeInfo, PieceLocation, PieceOrientation);
	}

	TArray<UCookieLandMapRangeInfo*> OutMapOmnibearingRangeInfos = CreateOrGetMapOmnibearingRangeInfos(PieceLocation);
	for (int Index = 0; Index < OutMapOmnibearingRangeInfos.Num(); ++Index)
	{
		UCookieLandMapRangeInfo* OutMapOmnibearingRangeInfo = OutMapOmnibearingRangeInfos[Index];
		if (bIsAdd)
		{
			OutMapOmnibearingRangeInfo->AddPiece(PieceLocation);
		}
		else
		{
			MapRangeInfoTryRemove(OutMapOmnibearingRangeInfo, PieceLocation, PieceOrientation);
		}
	}
}

void UCookieLandMapBuilder::MapRangeInfoTryRemove(UCookieLandMapRangeInfo* MapRangeInfo, const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	TArray< ECookieLandPieceOrientation> Orientations;
	switch (PieceOrientation)
	{
	case ECookieLandPieceOrientation::Up:
	case ECookieLandPieceOrientation::Down:
		Orientations.Add(ECookieLandPieceOrientation::Up);
		Orientations.Add(ECookieLandPieceOrientation::Down);
		break;
	case ECookieLandPieceOrientation::Left:
	case ECookieLandPieceOrientation::Right:
		Orientations.Add(ECookieLandPieceOrientation::Left);
		Orientations.Add(ECookieLandPieceOrientation::Right);
		break;
	case ECookieLandPieceOrientation::Forward:
	case ECookieLandPieceOrientation::Backward:
		Orientations.Add(ECookieLandPieceOrientation::Forward);
		Orientations.Add(ECookieLandPieceOrientation::Backward);
		break;
	};

	// 地块被移除时，自动移除强制绑定
	PieceDeleteForceLine(PieceLocation, PieceOrientation);

	bool bForceRemove = true;
	for (int Index = 0; Index < Orientations.Num(); ++Index)
	{
		// 因为存储时是不分双方向，所以移除时需要双方向判断确认
		ECookieLandPieceOrientation Orientation = Orientations[Index];
		if (GetIsPieceOccupyByLocator(PieceLocation, Orientation))
		{
			bForceRemove = false;
		}
	}

	if (bForceRemove)
	{
		MapRangeInfo->RemovePiece(PieceLocation);
	}
}

TArray<UCookieLandMapRangeInfo*> UCookieLandMapBuilder::CreateOrGetMapOmnibearingRangeInfos(const FCookieLandLocation PieceLocation)
{
	TArray<UCookieLandMapRangeInfo*> OutMapOmnibearingRangeInfos;

	TArray< ECookieLandPieceOrientation> Orientations = { ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Forward };

	TArray<FCookieLandPieceLocator> Locators;
	for (int Index = 0; Index < Orientations.Num(); ++Index)
	{
		ECookieLandPieceOrientation Orientation = Orientations[Index];
		Locators.Add(FCookieLandPieceLocator(PieceLocation, Orientation));
	}

	for (int Index = 0; Index < MapOmnibearingRangeInfos.Num(); ++Index)
	{
		UCookieLandMapRangeInfo* MapRangeInfo = MapOmnibearingRangeInfos[Index];
		for (int J = 0; J < Locators.Num(); ++J) 
		{
			FCookieLandPieceLocator Locator = Locators[J];
			if (MapRangeInfo->GetIsConformanceRange(Locator.PieceLocation, Locator.PieceOrientation))
			{
				OutMapOmnibearingRangeInfos.Add(MapRangeInfo);
				Locators.RemoveAt(J);
				break;
			}
		}
		if (Locators.Num() == 0)
		{
			break;
		}
	}

	for (int J = 0; J < Locators.Num(); ++J)
	{
		FCookieLandPieceLocator Locator = Locators[J];

		UCookieLandMapRangeInfo* MapRangeInfo = NewObject<UCookieLandMapRangeInfo>();
		MapRangeInfo->Init(Locator.PieceLocation, Locator.PieceOrientation);

		MapOmnibearingRangeInfos.Add(MapRangeInfo);
		OutMapOmnibearingRangeInfos.Add(MapRangeInfo);
	}

	return OutMapOmnibearingRangeInfos;
}

FCookieLandOrientationLinkInfo UCookieLandMapBuilder::ExecutePerceptualObjectFindForceLinkInfoEventCallback(FCookieLandPieceLocator Locator)
{
	FCookieLandOrientationLinkInfo ForeceLineInfo;
	GetForceLineInfo(Locator.PieceLocation, Locator.PieceOrientation, ForeceLineInfo);

	return ForeceLineInfo;
}