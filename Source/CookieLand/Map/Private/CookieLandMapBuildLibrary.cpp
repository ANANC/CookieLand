// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"
#include "CookieLand/Global/Public/CookieLandGlobal.h"
#include "CookieLand/Map/Public/CookieLandPiece.h"


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
		PieceLocation.X * InMapBuildInfo.OrientationSortInterval->Value.X * (PieceOrientation == ECookieLandPieceOrientation::Forward ? 1 : PieceOrientation == ECookieLandPieceOrientation::Backward ? -1 : 0),
		PieceLocation.Y * InMapBuildInfo.OrientationSortInterval->Value.Y * (PieceOrientation == ECookieLandPieceOrientation::Left ? 1 : PieceOrientation == ECookieLandPieceOrientation::Right ? -1 : 0),
		PieceLocation.Floor * InMapBuildInfo.OrientationSortInterval->Value.Z * (PieceOrientation == ECookieLandPieceOrientation::Up ? 1 : PieceOrientation == ECookieLandPieceOrientation::Down ? -1 : 0));

	PieceInstanceLocation += InitLocation;

	return PieceInstanceLocation;
}

FQuat UCookieLandMapBuildLibrary::CalculatePieceActorInstanceRotation(const FCookieLandMapBuildInfo& InMapBuildInfo, const ECookieLandPieceOrientation& PieceOrientation)
{
	FRotator Rotator;

	switch(PieceOrientation)
	{
	case ECookieLandPieceOrientation::Left:
		Rotator = FRotator(270, 0, 0);
	case ECookieLandPieceOrientation::Right:
		Rotator = FRotator(90, 0, 0);
	case ECookieLandPieceOrientation::Up:
		Rotator = FRotator(0, 0, 0);
	case ECookieLandPieceOrientation::Down:
		Rotator = FRotator(0, 180, 0);
	case ECookieLandPieceOrientation::Forward:
		Rotator = FRotator(0, 270, 0);
	case ECookieLandPieceOrientation::Backward:
		Rotator = FRotator(0, 90, 0);
	};
	
	FQuat Quat = FQuat(Rotator);

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