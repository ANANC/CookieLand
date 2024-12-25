// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapBuildLibrary.h"
#include "CookieLand/Global/CookieLandGlobal.h"
#include "CookieLandPiece.h"
#include "CookieLand/PerceptualObject/CookieLandPerceptualObjectSubsystem.h"
#include "CookieLandMapSubsystem.h"

UCookieLandPerceptualObjectSubsystem* UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem()
{
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = Cast< UCookieLandPerceptualObjectSubsystem>(UCookieLandGlobal::Get().FindSubsystem(UCookieLandPerceptualObjectSubsystem::StaticClass()));
	return PerceptualObjectSubsystem;
}

UCookieLandMapSubsystem* UCookieLandMapBuildLibrary::GetMapSubsystem()
{
	UCookieLandMapSubsystem* MapSubsystem = Cast< UCookieLandMapSubsystem>(UCookieLandGlobal::Get().FindSubsystem(UCookieLandMapSubsystem::StaticClass()));
	return MapSubsystem;
}

bool UCookieLandMapBuildLibrary::GetSourceMapBuildDataTable(FName MapName, FCookieLandMapBuildDataTableRow& OutMapBuildDataTableRow)
{
	UDataTable* MapBuildDataTable = UCookieLandGlobal::Get().GetGameData().MapBuildDataTable;
	ensure(MapBuildDataTable);

	FCookieLandMapBuildDataTableRow* Row = MapBuildDataTable->FindRow<FCookieLandMapBuildDataTableRow>(MapName, TEXT("FCookieLandMapBuildDataTableRow"));
	if (Row)
	{
		OutMapBuildDataTableRow = *Row;
		return true;
	}
	return false;
}

bool UCookieLandMapBuildLibrary::GetAmendMapBuildDataTable(FName MapName, FCookieLandMapBuildDataTableRow& OutMapBuildDataTableRow)
{
	if (!GetSourceMapBuildDataTable(MapName, OutMapBuildDataTableRow))
	{
		return false;
	}

	FCookieLandMapBuildDataTableRow DefaultMapBuildData;
	if (GetSourceMapBuildDataTable("Default", DefaultMapBuildData))
	{
		if (!OutMapBuildDataTableRow.BuildDataAsset || !DefaultMapBuildData.BuildDataAsset)
		{
			return false;
		}

		TArray< UCookieLandBaseConfigInfo*> Propertys = OutMapBuildDataTableRow.BuildDataAsset->BuildInfo.GetPropertys();
		TArray< UCookieLandBaseConfigInfo*> DefaultPropertys = DefaultMapBuildData.BuildDataAsset->BuildInfo.GetPropertys();
		for (int Index = 0; Index < Propertys.Num(); ++Index)
		{
			UCookieLandBaseConfigInfo* Property = Propertys[Index];
			UCookieLandBaseConfigInfo* DefaultProperty = DefaultPropertys[Index];

			if (!Property || !DefaultProperty)
			{
				continue;
			}

			Propertys[Index]->CopyWhenUseDefault(DefaultPropertys[Index]);
		}
	}

	return true;
}


ACookieLandPieceActor* UCookieLandMapBuildLibrary::CreatePieceActorInstanceByMapName(const UObject* ObjectInstance, FName MapName, const FCookieLandPieceBuildInfo& InBuildInfo)
{
	if (!ObjectInstance)
	{
		return nullptr;
	}
	if (!InBuildInfo.PieceActorType)
	{
		return nullptr;
	}

	FCookieLandMapBuildDataTableRow TargetMapBuildData;
	if (!GetAmendMapBuildDataTable(MapName, TargetMapBuildData))
	{
		return nullptr;
	}

	if (!TargetMapBuildData.BuildDataAsset)
	{
		return nullptr;
	}

	ACookieLandPieceActor* InstanceActor = CreatePieceActorInstanceByBuildInfo(ObjectInstance, TargetMapBuildData.BuildDataAsset->BuildInfo, InBuildInfo);
	return InstanceActor;
}
	
ACookieLandPieceActor* UCookieLandMapBuildLibrary::CreatePieceActorInstanceByBuildInfo(const UObject* ObjectInstance, const FCookieLandMapBuildInfo& InMapBuildInfo, const FCookieLandPieceBuildInfo& InBuildInfo)
{
	if (!ObjectInstance ||!ObjectInstance->GetWorld())
	{
		return nullptr;
	}

	TSubclassOf< ACookieLandPieceActor> PieceActorType = InBuildInfo.PieceActorType;
	if (!PieceActorType)
	{
		PieceActorType = InMapBuildInfo.PieceActorType->PieceActorType;
	}

	if (!PieceActorType)
	{
		return nullptr;
	}

	FTransform InitTransfrom;

	FVector InitLocation = CalculatePieceActorInstanceLocation(InMapBuildInfo, InBuildInfo.PieceLocation, InBuildInfo.PieceOrientation);
	InitTransfrom.SetLocation(InitLocation);

	FQuat Rotation = CalculatePieceActorInstanceRotation(InMapBuildInfo, InBuildInfo.PieceOrientation);
	InitTransfrom.SetRotation(Rotation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* InstanceActor = ObjectInstance->GetWorld()->SpawnActor(PieceActorType, &InitTransfrom, SpawnParameters);
	return Cast< ACookieLandPieceActor>(InstanceActor);
}

FVector UCookieLandMapBuildLibrary::CalculatePieceActorInstanceLocation(const FCookieLandMapBuildInfo& InMapBuildInfo, const FCookieLandLocation& PieceLocation, const ECookieLandPieceOrientation& PieceOrientation)
{
	FVector InitLocation = InMapBuildInfo.InitialLocation->Value;

	FVector PieceInstanceLocation = FVector(
		PieceLocation.X * InMapBuildInfo.LocationSortInterval->Value.X,
		PieceLocation.Y * InMapBuildInfo.LocationSortInterval->Value.Y,
		PieceLocation.Floor * InMapBuildInfo.LocationSortInterval->Value.Z);

	FVector PieceInstanceOrientation = FVector(
		InMapBuildInfo.OrientationSortInterval->Value.X * (PieceOrientation == ECookieLandPieceOrientation::Forward ? 1 : PieceOrientation == ECookieLandPieceOrientation::Backward ? -1 : 0),
		InMapBuildInfo.OrientationSortInterval->Value.Y * (PieceOrientation == ECookieLandPieceOrientation::Left ? 1 : PieceOrientation == ECookieLandPieceOrientation::Right ? -1 : 0),
		InMapBuildInfo.OrientationSortInterval->Value.Z * (PieceOrientation == ECookieLandPieceOrientation::Up ? 1 : PieceOrientation == ECookieLandPieceOrientation::Down ? -1 : 0));

	PieceInstanceLocation += InitLocation + PieceInstanceOrientation;

	return PieceInstanceLocation;
}

FQuat UCookieLandMapBuildLibrary::CalculatePieceActorInstanceRotation(const FCookieLandMapBuildInfo& InMapBuildInfo, const ECookieLandPieceOrientation& PieceOrientation)
{
	FRotator Rotator;

	switch(PieceOrientation)
	{
	case ECookieLandPieceOrientation::Left:
		Rotator = FRotator(0, 0, 90);
		break;
	case ECookieLandPieceOrientation::Right:
		Rotator = FRotator(0, 0, 270);
		break;
	case ECookieLandPieceOrientation::Up:
		Rotator = FRotator(0, 0, 0);
		break;
	case ECookieLandPieceOrientation::Down:
		Rotator = FRotator(0, 180, 0);
		break;
	case ECookieLandPieceOrientation::Forward:
		Rotator = FRotator(270, 0, 0);
		break;
	case ECookieLandPieceOrientation::Backward:
		Rotator = FRotator(90, 0, 0);
		break;
	};
	
	FQuat Quat = Rotator.Quaternion();

	return Quat;
}

TArray< FCookieLandPieceBuildInfo> UCookieLandMapBuildLibrary::CrateCubeBuildInfo(const FCookieLandLocation& PieceLocation)
{
	TArray< FCookieLandPieceBuildInfo> PieceBuildInfos
	{
		CratePieceBuildInfo(PieceLocation,ECookieLandPieceOrientation::Up),
		CratePieceBuildInfo(PieceLocation,ECookieLandPieceOrientation::Down),
		CratePieceBuildInfo(PieceLocation,ECookieLandPieceOrientation::Left),
		CratePieceBuildInfo(PieceLocation,ECookieLandPieceOrientation::Right),
		CratePieceBuildInfo(PieceLocation,ECookieLandPieceOrientation::Forward),
		CratePieceBuildInfo(PieceLocation,ECookieLandPieceOrientation::Backward),
	};

	return PieceBuildInfos;
}

FCookieLandPieceBuildInfo UCookieLandMapBuildLibrary::CratePieceBuildInfo(const FCookieLandLocation& PieceLocation, const ECookieLandPieceOrientation& PieceOrientation)
{
	return FCookieLandPieceBuildInfo(PieceLocation, PieceOrientation);
}

bool UCookieLandMapBuildLibrary::GetPerceptualObjectPerceptionDataTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionDataTableRow& OutPerceptualObjectPerceptionDataTableRow)
{
	UDataTable* PerceptualObjectPerceptionDataTable = UCookieLandGlobal::Get().GetGameData().PerceptualObjectPerceptionDataTable;
	ensure(PerceptualObjectPerceptionDataTable);

	FCookieLandPerceptualObjectPerceptionDataTableRow* Row = PerceptualObjectPerceptionDataTable->FindRow<FCookieLandPerceptualObjectPerceptionDataTableRow>(PerceptualObjectType, TEXT("FCookieLandPerceptualObjectPerceptionDataTableRow"));
	if (Row)
	{
		OutPerceptualObjectPerceptionDataTableRow = *Row;
		return true;
	}
	return false;
}

bool UCookieLandMapBuildLibrary::GetPerceptualObjectPerceptionInfoFromTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionInfo& OutPerceptualObjectPerceptionInfo)
{
	FCookieLandPerceptualObjectPerceptionDataTableRow PerceptualObjectPerceptionDataTableRow;
	if (GetPerceptualObjectPerceptionDataTable(PerceptualObjectType, PerceptualObjectPerceptionDataTableRow))
	{
		OutPerceptualObjectPerceptionInfo = PerceptualObjectPerceptionDataTableRow.PerceptionInfo;
		return true;
	}

	return false;
}

TArray<ECookieLandPieceOrientation> UCookieLandMapBuildLibrary::GetMapAgnleViewOrientations(ECookieLandMapAngleViewType MapAngleViewType, const ECookieLandPieceOrientation& PieceOrientation)
{
	switch (PieceOrientation)
	{
	case ECookieLandPieceOrientation::Up:
		switch (MapAngleViewType)
		{
		case ECookieLandMapAngleViewType::ForwardAndRight:
			return { ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Right};
		case ECookieLandMapAngleViewType::ForwardAndLeft:
			return { ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Left };
		case ECookieLandMapAngleViewType::BackwardAndRight:
			return { ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Right };
		case ECookieLandMapAngleViewType::BackwardAndLeft:
			return { ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Left };;
		}
		return {};
	case ECookieLandPieceOrientation::Down:
		switch (MapAngleViewType)
		{
		case ECookieLandMapAngleViewType::ForwardAndRight:
			return { ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Right };
		case ECookieLandMapAngleViewType::ForwardAndLeft:
			return { ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Left };
		case ECookieLandMapAngleViewType::BackwardAndRight:
			return { ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Right };
		case ECookieLandMapAngleViewType::BackwardAndLeft:
			return { ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Left };
		}
		return {};
	case ECookieLandPieceOrientation::Left:
		switch (MapAngleViewType)
		{
		case ECookieLandMapAngleViewType::ForwardAndRight:
			return { ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Forward };
		case ECookieLandMapAngleViewType::ForwardAndLeft:
			return { ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Backward };
		case ECookieLandMapAngleViewType::BackwardAndRight:
			return { ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Forward };
		case ECookieLandMapAngleViewType::BackwardAndLeft:
			return { ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Backward };
		}
		return {};
	case ECookieLandPieceOrientation::Right:
		switch (MapAngleViewType)
		{
		case ECookieLandMapAngleViewType::ForwardAndRight:
			return { ECookieLandPieceOrientation::Right,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Backward };
		case ECookieLandMapAngleViewType::ForwardAndLeft:
			return { ECookieLandPieceOrientation::Right,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Forward };
		case ECookieLandMapAngleViewType::BackwardAndRight:
			return { ECookieLandPieceOrientation::Right,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Backward };
		case ECookieLandMapAngleViewType::BackwardAndLeft:
			return { ECookieLandPieceOrientation::Right,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Forward };
		}
		return {};
	case ECookieLandPieceOrientation::Forward:
		switch (MapAngleViewType)
		{
		case ECookieLandMapAngleViewType::ForwardAndRight:
			return { ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Right };
		case ECookieLandMapAngleViewType::ForwardAndLeft:
			return { ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Left };
		case ECookieLandMapAngleViewType::BackwardAndRight:
			return { ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Right };
		case ECookieLandMapAngleViewType::BackwardAndLeft:
			return { ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Left };
		}
		return {};
	case ECookieLandPieceOrientation::Backward:
		switch (MapAngleViewType)
		{
		case ECookieLandMapAngleViewType::ForwardAndRight:
			return { ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Right };
		case ECookieLandMapAngleViewType::ForwardAndLeft:
			return { ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Left };
		case ECookieLandMapAngleViewType::BackwardAndRight:
			return { ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Right };
		case ECookieLandMapAngleViewType::BackwardAndLeft:
			return { ECookieLandPieceOrientation::Backward,ECookieLandPieceOrientation::Down,ECookieLandPieceOrientation::Left };
		}
		return {};
	default:
		return {};
	}
}

void UCookieLandMapBuildLibrary::GetRectPieceLocators(TArray< FCookieLandPieceLocator>& OutLocators, FCookieLandPieceLocator CenterLocator, int FloorDistance, bool bNeedOtherFloorLocator, int RoundDistance)
{
	TArray< ECookieLandPieceOrientation> Floor{ ECookieLandPieceOrientation::Up,ECookieLandPieceOrientation::Down };
	TArray< TArray<ECookieLandPieceOrientation>> Round{
		{ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Forward},
		{ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Backward},
		{ECookieLandPieceOrientation::Right,ECookieLandPieceOrientation::Forward},
		{ECookieLandPieceOrientation::Right,ECookieLandPieceOrientation::Backward},
	};

	TArray< FCookieLandPieceLocator> CenterLocators{ CenterLocator };

	if (FloorDistance > 0)
	{
		for (int FloorIndex = 0; FloorIndex < FloorDistance; ++FloorIndex)
		{
			for (int FloorOrientationIndex = 0; FloorOrientationIndex < Floor.Num(); ++FloorOrientationIndex)
			{
				ECookieLandPieceOrientation FloorOrientation = Floor[FloorOrientationIndex];

				FCookieLandPieceLocator FloorLocator(CenterLocator);
				FloorLocator.PieceLocation.AddDistanceBySixDirection(FloorOrientation, FloorIndex);
				
				if (bNeedOtherFloorLocator && RoundDistance > 0)
				{
					CenterLocators.Add(FloorLocator);
				}
				else
				{
					OutLocators.AddUnique(FloorLocator);
				}
			}
		}
	}

	for (int CenterIndex = 0; CenterIndex < CenterLocators.Num(); ++CenterIndex)
	{
		FCookieLandPieceLocator Locator = CenterLocators[CenterIndex];
		OutLocators.AddUnique(Locator);

		for (int RoundDistanceIndex = 0; RoundDistanceIndex < RoundDistance; ++RoundDistanceIndex)
		{
			for (int RoundIndex = 0; RoundIndex < Round.Num(); ++RoundIndex)
			{
				FCookieLandPieceLocator RoundLocator(CenterLocator);

				TArray<ECookieLandPieceOrientation> RoundOrientations = Round[RoundIndex];
				for (int RoundOrientationsIndex = 0; RoundOrientationsIndex < RoundOrientations.Num(); ++RoundOrientationsIndex)
				{
					ECookieLandPieceOrientation RoundOrientation = RoundOrientations[RoundOrientationsIndex];
					RoundLocator.PieceLocation.AddDistanceBySixDirection(RoundOrientation, RoundIndex);
				}
				OutLocators.AddUnique(RoundLocator);
			}
		}
	}
}

bool UCookieLandMapBuildLibrary::GetEnableMoveUpByMyself(const FCookieLandPieceBaseAction& BaseAction)
{
	for (int Index = 0; Index < BaseAction.EnableOrientations.Num(); ++Index)
	{
		ECookieLandPieceOrientation EnablePieceOrientation = BaseAction.EnableOrientations[Index];
		if (EnablePieceOrientation == ECookieLandPieceOrientation::Up)
		{
			return true;
		}
	}

	return false;
}

bool UCookieLandMapBuildLibrary::GetEnableMoveDownByMyself(const FCookieLandPieceBaseAction& BaseAction)
{
	for (int Index = 0; Index < BaseAction.EnableOrientations.Num(); ++Index)
	{
		ECookieLandPieceOrientation EnablePieceOrientation = BaseAction.EnableOrientations[Index];
		if (EnablePieceOrientation == ECookieLandPieceOrientation::Down)
		{
			return true;
		}
	}

	return false;
}

bool UCookieLandMapBuildLibrary::GetPerceptionInfoDataTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionDataTableRow& OutPerceptionInfoDataTableRow)
{
	UDataTable* PerceptualObjectPerceptionDataTable = UCookieLandGlobal::Get().GetGameData().PerceptualObjectPerceptionDataTable;
	ensure(PerceptualObjectPerceptionDataTable);

	FCookieLandPerceptualObjectPerceptionDataTableRow* Row = PerceptualObjectPerceptionDataTable->FindRow<FCookieLandPerceptualObjectPerceptionDataTableRow>(PerceptualObjectType, TEXT("FCookieLandPerceptualObjectPerceptionDataTableRow"));
	if (Row)
	{
		OutPerceptionInfoDataTableRow = *Row;
		return true;
	}
	return false;
}