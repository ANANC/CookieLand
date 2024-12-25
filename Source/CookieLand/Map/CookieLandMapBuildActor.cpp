// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapBuildActor.h"
#include "CookieLandMapBuildLibrary.h"
#include "CookieLandPiece.h"
#include "CookieLandMapBuilder.h"
#include "CookieLandMapActorGather.h"
#include "CookieLandMapShowDirector.h"
#include "CookieLandMapSubsystem.h"
#include "CookieLand/PerceptualObject/CookieLandPerceptualObjectSubsystem.h"

// Sets default values
ACookieLandMapBuildActor::ACookieLandMapBuildActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	MapBuilder = CreateDefaultSubobject<UCookieLandMapBuilder>("MapBuilder");
	MapActorGather = CreateDefaultSubobject<UCookieLandMapActorGather>("MapActorGather");
	ShowDirector = CreateDefaultSubobject<UCookieLandMapShowDirector>("ShowDirector");

	ShowDirector->SetBuildActor(this);

	MapActorGather->MapBuildInfo = MapBuildInfo;
	ShowDirector->MapShowInfo = MapShowInfo;
}


// Called when the game starts or when spawned
void ACookieLandMapBuildActor::BeginPlay()
{
	Super::BeginPlay();

	UCookieLandMapSubsystem* MapSubsystem = UCookieLandMapBuildLibrary::GetMapSubsystem();
	if (MapSubsystem) 
	{
		MapSubsystem->RegisterMapBuildActor(this);
	}
}

void ACookieLandMapBuildActor::BeginDestroy()
{
	UCookieLandMapSubsystem* MapSubsystem = UCookieLandMapBuildLibrary::GetMapSubsystem();
	if (MapSubsystem) 
	{
		MapSubsystem->UnRegisterMapBuildActor(this);
	}

	Super::BeginDestroy();
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
		else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(ACookieLandMapBuildActor, MapShowInfo))
		{
			if (ShowDirector)
			{
				ShowDirector->MapShowInfo = MapShowInfo;
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
			MapActorGather->MapBuildInfo = TargetMapBuildData.BuildDataAsset->BuildInfo;

			MapShowInfo = TargetMapBuildData.ShowDataAsset->MapShowInfo;
			ShowDirector->MapShowInfo = TargetMapBuildData.ShowDataAsset->MapShowInfo;
		}
	}
}

void ACookieLandMapBuildActor::CreateEnvironment()
{
	// 加载地形数据
	ReloadMapBuildInfo();

}

void ACookieLandMapBuildActor::EnterThisMap(bool bDestroyLastMap)
{
	UCookieLandMapSubsystem* MapSubsystem = UCookieLandMapBuildLibrary::GetMapSubsystem();

	MapSubsystem->EnterMap(MapName, bDestroyLastMap);

	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
	PerceptualObjectSubsystem->PerceptualObjectLocatorChangeEvent.AddDynamic(ShowDirector, &UCookieLandMapShowDirector::ReceivePerceptualObjectLocatorChangeEventCallback);
	PerceptualObjectSubsystem->PerceptualObjectFindForceLinkInfoEvent.BindUObject(MapBuilder ,&UCookieLandMapBuilder::ExecutePerceptualObjectFindForceLinkInfoEventCallback);
}

void ACookieLandMapBuildActor::ExistThisMap(bool bNeedDestroy)
{
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
	PerceptualObjectSubsystem->PerceptualObjectLocatorChangeEvent.RemoveDynamic(ShowDirector, &UCookieLandMapShowDirector::ReceivePerceptualObjectLocatorChangeEventCallback);
	PerceptualObjectSubsystem->PerceptualObjectFindForceLinkInfoEvent.Unbind();

	if (bNeedDestroy)
	{
		DestryEnvironment();
	}
}

void ACookieLandMapBuildActor::DestryEnvironment()
{
	// 销毁地块
	DeleteAllPiece();
}

void ACookieLandMapBuildActor::CreateCube(const FCookieLandLocation CubeLoaction)
{
	TArray< FCookieLandPieceBuildInfo> PieceBuildInfos = UCookieLandMapBuildLibrary::CrateCubeBuildInfo(CubeLoaction);

	for (int Index = 0; Index < PieceBuildInfos.Num(); ++Index)
	{
		FCookieLandPieceBuildInfo PieceBuildInfo = PieceBuildInfos[Index];
		CreatePiece(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation);
	}
}

void ACookieLandMapBuildActor::CreatePiece(const FCookieLandLocation PieceLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (MapBuilder->OccupyPieceByLocation(PieceLocation, PieceOrientation))
	{
		FCookieLandPieceBuildInfo PieceBuildInfo = UCookieLandMapBuildLibrary::CratePieceBuildInfo(PieceLocation, PieceOrientation);
		UCookieLandPiece* Piece = MapActorGather->AddPiece(PieceBuildInfo);
		if (!Piece)
		{
			MapBuilder->ReleasePieceByLocation(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation);
			return;
		}

		Piece->SetMapBuildActor(this);

		if (PieceBuildInfo.bAutoCreateActorInstance)
		{
			TryCreatePieceActorToPiece(PieceLocation, PieceOrientation);
		}
	}
}

void ACookieLandMapBuildActor::DeleteCube(const FCookieLandLocation CubeLoaction)
{
	TArray< FCookieLandPieceBuildInfo> PieceBuildInfos = UCookieLandMapBuildLibrary::CrateCubeBuildInfo(CubeLoaction);

	for (int Index = 0; Index < PieceBuildInfos.Num(); ++Index)
	{
		FCookieLandPieceBuildInfo PieceBuildInfo = PieceBuildInfos[Index];

		DeletePiece(PieceBuildInfo.PieceLocation, PieceBuildInfo.PieceOrientation);
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
			FVector Location = UCookieLandMapBuildLibrary::CalculatePieceActorInstanceLocation(MapActorGather->MapBuildInfo, Piece->GetPieceLocation(), Piece->GetPieceOrientation());
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


bool ACookieLandMapBuildActor::TryCreatePieceActorToPiece(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	UCookieLandPiece* Piece = MapActorGather->GetPiece(MapCubeLocation, PieceOrientation);
	if (!Piece)
	{
		return false;
	}

	if (Piece->GetPieceAction())
	{
		return true;
	}

	ACookieLandPieceActor* ActorInstance = UCookieLandMapBuildLibrary::CreatePieceActorInstanceByBuildInfo(this, MapActorGather->MapBuildInfo, Piece->GetBuildInfo());
	if (ActorInstance)
	{
		ActorInstance->Init(Piece);
		ActorInstance->SetMapShowDirector(ShowDirector);

		Piece->SetPieceActor(ActorInstance);

		return true;
	}

	return false;
}

void ACookieLandMapBuildActor::ChangePieceActorType(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation, TSubclassOf< ACookieLandPieceActor> InPieceActorType)
{
	UCookieLandPiece* Piece = MapActorGather->GetPiece(MapCubeLocation, PieceOrientation);
	if (!Piece)
	{
		return;
	}

	if (ACookieLandPieceActor* ActorInstance = Piece->GetPieceAction())
	{
		if (ActorInstance && ActorInstance->GetClass() == InPieceActorType)
		{
			return;
		}

		if (ActorInstance && ActorInstance->GetClass() != InPieceActorType)
		{
			Piece->SetPieceActor(nullptr);
			ActorInstance->Destroy();
		}
	}

	Piece->SetPieceActorType(InPieceActorType);
	TryCreatePieceActorToPiece(MapCubeLocation, PieceOrientation);
}