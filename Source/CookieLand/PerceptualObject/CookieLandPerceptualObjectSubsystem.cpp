// Fill out your copyright notice in the Description page of Project Settings.

#include "CookieLandPerceptualObjectSubsystem.h"
#include "CookieLandMapBuildLibrary.h"

#define Max(a, b) ((a) > (b) ? (a) : (b))

int UCookieLandPerceptualObjectSubsystem::AddPerceptualObject(bool bMainPerceptualObject, FName InPerceptualObjectType, bool bInEnablePerceptual)
{
	UCookieLandPerceptualObject* PerceptualObject = CreatePerceptualObject();
	PerceptualObject->PerceptualObjectType = InPerceptualObjectType;
	PerceptualObject->bEnablePerceptual = bInEnablePerceptual;

	GetPerceptionInfoFromTable(InPerceptualObjectType, PerceptualObject->PerceptionInfo);

	if (bMainPerceptualObject)
	{
		MainPerceptualObject = PerceptualObject;
	}

	return PerceptualObject->Id;
}

void UCookieLandPerceptualObjectSubsystem::RemovePerceptualObject(int Id)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	PerceptualObjects.Remove(PerceptualObject);

	UpdatePassivePerceptualObjectLocators();
}

void UCookieLandPerceptualObjectSubsystem::UpdatePerceptualObjectLocation(int Id, FCookieLandLocation PieceLocation)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	UpdatePerceptualObjectLocator(Id, FCookieLandPieceLocator(PieceLocation, PerceptualObject->PieceOrientation));
}

void UCookieLandPerceptualObjectSubsystem::UpdatePerceptualObjectLocator(int Id, FCookieLandPieceLocator PieceLocator)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	FCookieLandPieceLocator OldPieceLocator = FCookieLandPieceLocator(PerceptualObject->PieceLocation, PerceptualObject->PieceOrientation);

	PerceptualObject->PieceLocation = PieceLocator.PieceLocation;
	PerceptualObject->PieceOrientation = PieceLocator.PieceOrientation;

	UpdatePassivePerceptualObjectLocators();

	if (PerceptualObject == MainPerceptualObject)
	{
		MainPerceptualObjectLocatorChangeEvent.Broadcast(PerceptualObject->Id, OldPieceLocator, PieceLocator);
	}
	PerceptualObjectLocatorChangeEvent.Broadcast(PerceptualObject->Id, OldPieceLocator, PieceLocator);
}

void UCookieLandPerceptualObjectSubsystem::UpdatePerceptualObjectEnablePerceptual(int Id, bool bInEnablePerceptual)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	PerceptualObject->bEnablePerceptual = bInEnablePerceptual;

	UpdatePassivePerceptualObjectLocators();
}

void UCookieLandPerceptualObjectSubsystem::UpdatePerceptualObjectPerceptionInfo(int Id, FCookieLandPerceptualObjectPerceptionInfo InPerceptionInfo)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	PerceptualObject->PerceptionInfo = InPerceptionInfo;

	UpdatePassivePerceptualObjectLocators();

	PerceptualObjectPerceptionInfoUpateEvent.Broadcast(PerceptualObject->Id);
}

UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::CreatePerceptualObject()
{
	UCookieLandPerceptualObject* PerceptualObject = NewObject<UCookieLandPerceptualObject>();
	PerceptualObject->Id = AutoId++;

	PerceptualObjects.Add(PerceptualObject);

	return PerceptualObject;
}

UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::FindPerceptualObject(int Id)
{
	for (int Index = 0; Index < PerceptualObjects.Num(); ++Index)
	{
		UCookieLandPerceptualObject* PerceptualObject = PerceptualObjects[Index];
		if (PerceptualObject->Id == Id)
		{
			return PerceptualObject;
		}
	}

	return nullptr;
}

const UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::GetMainPerceptualObject()
{
	return MainPerceptualObject;
}

const UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::GetPerceptualObject(int Id)
{
	return FindPerceptualObject(Id);
}

bool UCookieLandPerceptualObjectSubsystem::GetMainCurrentLocator(FCookieLandPieceLocator& MainLocator)
{
	if (MainPerceptualObject)
	{
		MainLocator = FCookieLandPieceLocator(MainPerceptualObject->PieceLocation, MainPerceptualObject->PieceOrientation);
		return true;
	}
	return false;
}

void UCookieLandPerceptualObjectSubsystem::SetMapAngleViewType(ECookieLandMapAngleViewType InMapAngleViewType)
{
	MapAngleViewType = InMapAngleViewType;
}

ECookieLandMapAngleViewType UCookieLandPerceptualObjectSubsystem::GetMapAngleViewType()
{
	return MapAngleViewType;
}
void UCookieLandPerceptualObjectSubsystem::UpdatePassivePerceptualObjectLocators()
{
	PassivePerceptualObjectLocators.Empty();
	PerceptualingPerceptualObjectLocators.Empty();
	PerceptualingPerceptualObjectIds.Empty();

	if (!MainPerceptualObject)
	{
		return;
	}

	FCookieLandPieceLocator MainPerceptualObjectLocator = MainPerceptualObject->GetLocator();

	for (int Index = 0; Index < PerceptualObjects.Num(); ++Index)
	{
		UCookieLandPerceptualObject* PerceptualObject = PerceptualObjects[Index];
		if (PerceptualObject->bEnablePerceptual && PerceptualObject->Id!= MainPerceptualObject->Id)
		{
			FCookieLandPieceLocator PerceptualObjectLocator = PerceptualObject->GetLocator();

			PassivePerceptualObjectLocators.Add(PerceptualObjectLocator);

			if (GetMainPerceptualObjectEnablePerceivePassiveePerceptualObject(PerceptualObject))
			{
				PerceptualingPerceptualObjectIds.Add(PerceptualObject->Id);
				PerceptualingPerceptualObjectLocators.Add(PerceptualObjectLocator);
			}
		}
	}
}

bool UCookieLandPerceptualObjectSubsystem::GetMainPerceptualObjectEnablePerceivePassiveePerceptualObject(const UCookieLandPerceptualObject* PerceptualObject)
{
	if (!MainPerceptualObject)
	{
		return false;
	}
	
	bool bEnable = GetEnablePerceivePassivePerceptualObject(MainPerceptualObject->PerceptionInfo, PerceptualObject->PerceptionInfo,MainPerceptualObject->GetLocator(), PerceptualObject->GetLocator());
	return bEnable;
}

bool UCookieLandPerceptualObjectSubsystem::GetEnablePerceivePassivePerceptualObject(
	const FCookieLandPerceptualObjectPerceptionInfo& CenterPerceptionInfo,
	const FCookieLandPerceptualObjectPerceptionInfo& PerceptualObjectPerceptionInfo,
	FCookieLandPieceLocator CenterLocator,
	FCookieLandPieceLocator PerceptualObjectLocator)
{
	TMap<ECookieLandPieceOrientation, int> RelativeDistances = CenterLocator.PieceLocation.GetAbsRelativeDistances(PerceptualObjectLocator.PieceLocation);

	// 不能感知其他方向
	if (!CenterPerceptionInfo.bEnablePerceiveOtherOrientation)
	{
		if (CenterLocator.PieceOrientation != PerceptualObjectLocator.PieceOrientation)
		{
			return false;
		}
	}

	// 不能感知其他层
	if (!CenterPerceptionInfo.bEnablePerceiveOtherFloor && RelativeDistances[ECookieLandPieceOrientation::Left] > 0)
	{
		return false;
	}

	// 感知和被感知 取最大值
	int MaxPerceptionRange = Max(CenterPerceptionInfo.PerceptionRange, PerceptualObjectPerceptionInfo.PassivePerceptionRange);

	// 周围的感知范围
	if (RelativeDistances[ECookieLandPieceOrientation::Left] > MaxPerceptionRange || RelativeDistances[ECookieLandPieceOrientation::Forward] > MaxPerceptionRange)
	{
		return false;
	}

	// 异域感知
	if (PerceptualObjectFindForceLinkInfoEvent.IsBound())
	{
		FCookieLandOrientationLinkInfo CenterForceLinkInfo = PerceptualObjectFindForceLinkInfoEvent.Execute(CenterLocator);
		FCookieLandOrientationLinkInfo PerceptualObjectForceLinkInfo = PerceptualObjectFindForceLinkInfoEvent.Execute(PerceptualObjectLocator);

		// 不同异域
		if (CenterForceLinkInfo != PerceptualObjectForceLinkInfo)
		{	
			if (!CenterPerceptionInfo.bEnablePerceiveWhereInForeignMap)
			{
				return false;
			}
		}
	}

	return true;
}

bool UCookieLandPerceptualObjectSubsystem::GetMainPerceptualObjectEnablePerceiveLocator(FCookieLandPieceLocator PerceptualObjectLocator)
{
	if (!MainPerceptualObject)
	{
		return false;
	}

	bool bEnable = GetEnablePerceiveLocator(MainPerceptualObject->PerceptionInfo, MainPerceptualObject->GetLocator(), PerceptualObjectLocator);
	return bEnable;
}

bool UCookieLandPerceptualObjectSubsystem::GetEnablePerceiveLocator(const FCookieLandPerceptualObjectPerceptionInfo& CenterPerceptionInfo, FCookieLandPieceLocator CenterLocator, FCookieLandPieceLocator PerceptualObjectLocator)
{
	for (int Index = 0; Index < PerceptualingPerceptualObjectIds.Num(); ++Index)
	{
		int PerceptualingPerceptualObjectId = PerceptualingPerceptualObjectIds[Index];

		UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(PerceptualingPerceptualObjectId);
		if (!PerceptualObject)
		{
			continue;
		}

		FCookieLandPerceptualObjectSenseMapInfo SenseForeignMap = CenterPerceptionInfo.SenseForeignMap;
		FCookieLandPerceptualObjectSenseMapInfo SenseMapRound = CenterPerceptionInfo.SenseMapRound;

		bool bCheck = SenseForeignMap.bEnablePerceive || SenseMapRound.bEnablePerceive;
		bool bEnableSenseOtherFloor = SenseForeignMap.bEnableSenseOtherFloor || SenseMapRound.bEnableSenseOtherFloor;

		if (!SenseForeignMap.bEnablePerceive)
		{
			FCookieLandOrientationLinkInfo CenterForceLinkInfo = PerceptualObjectFindForceLinkInfoEvent.Execute(CenterLocator);
			FCookieLandOrientationLinkInfo PerceptualObjectForceLinkInfo = PerceptualObjectFindForceLinkInfoEvent.Execute(PerceptualObjectLocator);

			// 不同异域
			if (CenterForceLinkInfo != PerceptualObjectForceLinkInfo)
			{
				bCheck = false;
			}
		}

		if (!bCheck)
		{
			continue;
		}

		// 层级
		if (bEnableSenseOtherFloor)
		{
			// 超出层级
			if (abs(PerceptualObjectLocator.PieceLocation.Floor - CenterLocator.PieceLocation.Floor) > SenseMapRound.SenseMapFloorRange)
			{
				continue;
			}
		}
		else
		{
			// 非同层
			if (PerceptualObjectLocator.PieceLocation.Floor != CenterLocator.PieceLocation.Floor)
			{
				continue;
			}
		}

		// 超出周边
		if (abs(PerceptualObjectLocator.PieceLocation.X - CenterLocator.PieceLocation.X) > SenseMapRound.SenseMapRoundRange ||
			(abs(PerceptualObjectLocator.PieceLocation.Y - CenterLocator.PieceLocation.Y) > SenseMapRound.SenseMapRoundRange))
		{
			continue;
		}

		return true;
	}

	return false;
}

bool UCookieLandPerceptualObjectSubsystem::GetPerceptionInfoFromTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionInfo& PerceptionInfo)
{
	FCookieLandPerceptualObjectPerceptionDataTableRow OutPerceptionInfoDataTableRow;
	if (UCookieLandMapBuildLibrary::GetPerceptionInfoDataTable(PerceptualObjectType, OutPerceptionInfoDataTableRow))
	{
		PerceptionInfo = OutPerceptionInfoDataTableRow.PerceptionInfo;
	}
	return false;
}