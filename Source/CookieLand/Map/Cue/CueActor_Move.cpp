// Fill out your copyright notice in the Description page of Project Settings.


#include "CueActor_Move.h"

void ACueActor_Move::Init(UCookieLandBasePieceAction* InAction, UCookieLandBaseCueActorData* InBaseData)
{
	Super::Init(InAction, InBaseData);

	MoveData = Cast<UCookieLandCueData_Move>(InBaseData);
	if (MoveData)
	{
		StaticMesh_Up = GetComponentByName< UStaticMeshComponent>("Up");
		StaticMesh_Down = GetComponentByName< UStaticMeshComponent>("Down");

		if (StaticMesh_Up)
		{
			StaticMesh_Up->SetVisibility(MoveData->MoveOrientation == ECookieLandPieceOrientation::Up);
		}
		if (StaticMesh_Down)
		{
			StaticMesh_Down->SetVisibility(MoveData->MoveOrientation == ECookieLandPieceOrientation::Down);
		}
	}
}

void ACueActor_Move::UnInit()
{
	if (StaticMesh_Up)
	{
		StaticMesh_Up->SetVisibility(false);
	}
	if (StaticMesh_Down)
	{
		StaticMesh_Down->SetVisibility(false);
	}

	Super::UnInit();
}