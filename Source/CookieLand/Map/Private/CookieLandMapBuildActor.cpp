// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Public/CookieLandMapBuildActor.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"
#include "CookieLand/Map/Public/CookieLandPiece.h"
#include "CookieLand/Map/Public/CookieLandMapBuilder.h"
#include "CookieLand/Map/Public/CookieLandMapActorGather.h"

#if WITH_EDITOR
ACookieLandMapBuildActor* ACookieLandMapBuildActor::MapBuildActorInstance = nullptr;
#endif

// Sets default values
ACookieLandMapBuildActor::ACookieLandMapBuildActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	MapBuilder = CreateDefaultSubobject<UCookieLandMapBuilder>("MapBuilder");
	MapActorGather = CreateDefaultSubobject<UCookieLandMapActorGather>("MapActorGather");

	MapActorGather->MapBuildInfo = MapBuildInfo;
}


// Called when the game starts or when spawned
void ACookieLandMapBuildActor::BeginPlay()
{
	Super::BeginPlay();
}


#if WITH_EDITOR
void ACookieLandMapBuildActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.MemberProperty)
	{
		const FName MemberPropertyName = PropertyChangedEvent.MemberProperty->GetFName();
		if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(ACookieLandMapBuildActor, MapBuildInfo))
		{
			if (MapActorGather)
			{
				MapActorGather->MapBuildInfo = MapBuildInfo;
			}
		}
	}
}
#endif

void ACookieLandMapBuildActor::ReloadMapBuildInfo()
{
	FCookieLandMapBuildDataTableRow TargetMapBuildData;
	if (UCookieLandMapBuildLibrary::GetAmendMapBuildDataTable(MapName, TargetMapBuildData))
	{
		if (TargetMapBuildData.BuildDataAsset)
		{
			MapBuildInfo = TargetMapBuildData.BuildDataAsset->BuildInfo;
			MapActorGather->MapBuildInfo = MapBuildInfo;
		}
	}
}

void ACookieLandMapBuildActor::CreateEnvironment()
{
	// 加载地形数据
	ReloadMapBuildInfo();

}

void ACookieLandMapBuildActor::CreateCube(const FCookieLandLocation CubeLoaction)
{
	TArray< FCookieLandPieceBuildInfo> PieceBuildInfos = UCookieLandMapBuildLibrary::CrateCubeBuildInfo(CubeLoaction);

	for (int Index = 0; Index < PieceBuildInfos.Num(); ++Index)
	{
		FCookieLandPieceBuildInfo PieceBuildInfo = PieceBuildInfos[Index];

		if (MapBuilder->OccupyPieceByLocation(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation))
		{
			MapActorGather->AddPiece(PieceBuildInfo);
		}
	}
}

void ACookieLandMapBuildActor::CreatePiece(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (MapBuilder->OccupyPieceByLocation(PieceLocation, PieceOrientation))
	{
		FCookieLandPieceBuildInfo PieceBuildInfo = UCookieLandMapBuildLibrary::CratePieceBuildInfo(PieceLocation, PieceOrientation);
		MapActorGather->AddPiece(PieceBuildInfo);
	}
}

void ACookieLandMapBuildActor::DeleteCube(const FCookieLandLocation CubeLoaction)
{
	TArray< FCookieLandPieceBuildInfo> PieceBuildInfos = UCookieLandMapBuildLibrary::CrateCubeBuildInfo(CubeLoaction);

	for (int Index = 0; Index < PieceBuildInfos.Num(); ++Index)
	{
		FCookieLandPieceBuildInfo PieceBuildInfo = PieceBuildInfos[Index];

		if (MapBuilder->ReleasePieceByLocation(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation))
		{
			MapActorGather->RemovePiece(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation);
		}
	}
}

void ACookieLandMapBuildActor::DeletePiece(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (MapBuilder->ReleasePieceByLocation(PieceLocation, PieceOrientation))
	{
		MapActorGather->RemovePiece(PieceLocation, PieceOrientation);
	}
}

void ACookieLandMapBuildActor::DeleteTargetFloorAllPiece(int Floor)
{
	TArray< FCookieLandPieceLocator>PieceLocators = MapBuilder->GetAllOccupyPieceByTargetFloor(Floor);

	for (int Index = 0; Index < PieceLocators.Num(); ++Index)
	{
		FCookieLandPieceLocator PieceLocator = PieceLocators[Index];
		DeletePiece(PieceLocator.PieceLocation, PieceLocator.PieceOrientation);
	}
}

void ACookieLandMapBuildActor::DeleteAllPiece()
{
	TArray< FCookieLandPieceLocator>PieceLocators = MapBuilder->GetAllOccupyPiece();
	for (int Index = 0; Index < PieceLocators.Num(); ++Index)
	{
		FCookieLandPieceLocator PieceLocator = PieceLocators[Index];
		DeletePiece(PieceLocator.PieceLocation, PieceLocator.PieceOrientation);
	}
}

void ACookieLandMapBuildActor::UpdateAllPieceInstanceLocation()
{
	TArray<UCookieLandPiece*> Pieces = MapActorGather->GetAllPieces();
	for (int Index = 0; Index < Pieces.Num(); ++Index)
	{
		UCookieLandPiece* Piece = Pieces[Index];
		if (ACookieLandPieceActor* ActorInstance = Piece->GetPieceAction())
		{
			FVector Location = UCookieLandMapBuildLibrary::CalculatePieceActorInstanceLocation(MapBuildInfo, Piece->GetPieceLocation(), Piece->GetPieceOrientation());
			FTransform Transform = ActorInstance->GetActorTransform();
			Transform.SetLocation(Location);
			ActorInstance->SetActorTransform(Transform);
		}
	}
}

TArray<FCookieLandPieceBuildInfo> ACookieLandMapBuildActor::GetAllPieceBuildInfo()
{
	TArray<FCookieLandPieceBuildInfo> PieceBuildInfos;

	TArray< FCookieLandPieceLocator>PieceLocators = MapBuilder->GetAllOccupyPiece();
	for (int Index = 0; Index < PieceLocators.Num(); ++Index)
	{
		FCookieLandPieceLocator PieceLocator = PieceLocators[Index];
		UCookieLandPiece* Piece = MapActorGather->GetPiece(PieceLocator.PieceLocation, PieceLocator.PieceOrientation);
		if (!Piece) { continue; }

		FCookieLandPieceBuildInfo PieceBuildInfo = Piece->GetBuildInfo();
		PieceBuildInfo.PieceLocation = PieceLocator.PieceLocation;
		PieceBuildInfo.PieceOrientation = PieceLocator.PieceOrientation;

		PieceBuildInfos.Add(PieceBuildInfo);
	}

	return PieceBuildInfos;
}

bool ACookieLandMapBuildActor::GetIsCubeOccupyByLocation(const FCookieLandLocation MapCubeLocation)
{
	return MapBuilder->GetIsCubeOccupyByLocation(MapCubeLocation);
}

bool ACookieLandMapBuildActor::GetIsPieceOccupyByLocation(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	return MapBuilder->GetIsPieceOccupyByLocation(MapCubeLocation, PieceOrientation);
}

UCookieLandPiece* ACookieLandMapBuildActor::GetPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	return MapActorGather->GetPiece(MapCubeLocation, PieceOrientation);
}

void ACookieLandMapBuildActor::ChangePieceActorType(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation, TSubclassOf< ACookieLandPieceActor> InPieceActorType)
{
	MapActorGather->ChangePieceActorType(MapCubeLocation, PieceOrientation, InPieceActorType);
}

TArray<int> ACookieLandMapBuildActor::GetAllOccupyFloor()
{
	return MapBuilder->GetAllOccupyFloor();
}
