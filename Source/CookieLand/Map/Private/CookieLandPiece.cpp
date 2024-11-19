// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Public/CookieLandPiece.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"

void ACookieLandPieceActor::BeginDestroy()
{
	UnInit();
	Super::BeginDestroy();
}

void ACookieLandPieceActor::Init(UCookieLandPiece* InCookieLandPiece)
{
	if (!InCookieLandPiece)
	{
		return;
	}

	bInit = true;

	CookieLandPiece = InCookieLandPiece;

	PieceLocation = InCookieLandPiece->GetPieceLocation();
	PieceOrientation = InCookieLandPiece->GetPieceOrientation();

}

void ACookieLandPieceActor::UnInit()
{
	bInit = false;
	CookieLandPiece = nullptr;
}

bool ACookieLandPieceActor::GetIsInit()
{
	return bInit;
}


void UCookieLandPiece::Init(FCookieLandPieceBuildInfo InBuildInfo)
{
	bInit = true;
	BuildInfo = InBuildInfo;
	PieceLocation = BuildInfo.PieceLocation;
	PieceOrientation = BuildInfo.PieceOrientation;
}

void UCookieLandPiece::UnInit()
{
	bInit = false;

	if (PieceActor.Get())
	{
		PieceActor->Destroy();
	}
	PieceActor = nullptr;
}

void UCookieLandPiece::SetPieceActor(ACookieLandPieceActor* Instance)
{
	PieceActor = Instance;
}

void UCookieLandPiece::SetPieceActorType(TSubclassOf< ACookieLandPieceActor> PieceActorType)
{
	BuildInfo.PieceActorType = PieceActorType;
}

ACookieLandPieceActor* UCookieLandPiece::GetPieceAction()
{
	return PieceActor;
}

FCookieLandLocation UCookieLandPiece::GetPieceLocation()
{
	return PieceLocation;
}

ECookieLandPieceOrientation UCookieLandPiece::GetPieceOrientation()
{
	return PieceOrientation;
}

FCookieLandPieceBuildInfo UCookieLandPiece::GetBuildInfo()
{
	return BuildInfo;
}