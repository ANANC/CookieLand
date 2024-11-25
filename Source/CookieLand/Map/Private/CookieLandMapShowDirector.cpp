// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapShowDirector.h"
#include "CookieLand/Map/Public/CookieLandMapBuildActor.h"
#include "CookieLand/Map/Public/CookieLandMapBuilder.h"

void UCookieLandMapShowDirector::SetBuildActor(ACookieLandMapBuildActor* InBuildActor)
{
	BuildActor = InBuildActor;
}

bool UCookieLandMapShowDirector::GetEnableDisplay(const FCookieLandLocation MapCubeLocation, const ECookieLandPieceOrientation PieceOrientation)
{
	if (!BuildActor || !BuildActor->GetMapBuilder())
	{
		return false;
	}
	
	if (!BuildActor->GetMapBuilder()->GetIsPieceOccupyByLocation(MapCubeLocation, PieceOrientation))
	{
		return false;
	}
	


	return false;
}