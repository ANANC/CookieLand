// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandBaseCueActor.h"
#include "CookieLandBasePieceAction.h"

// Sets default values
ACookieLandBaseCueActor::ACookieLandBaseCueActor()
{
}

// Called when the game starts or when spawned
void ACookieLandBaseCueActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACookieLandBaseCueActor::BeginDestroy()
{
	Super::BeginDestroy();

	if (Action)
	{
		Action->RemoveCueActor(this);
	}
}

void ACookieLandBaseCueActor::Init(UCookieLandBasePieceAction* InAction, UCookieLandBaseCueActorData* InBaseData)
{
	if (bInit)
	{
		return;
	}

	bInit = true;
	bUnInit = false;

	Action = InAction;
	BaseData = InBaseData;
}

void ACookieLandBaseCueActor::UnInit()
{
	if (bUnInit)
	{
		return;
	}

	bUnInit = true;

	Action = nullptr;
	BaseData = nullptr;
}

UCookieLandBasePieceAction* ACookieLandBaseCueActor::GetAction()
{
	return Action.Get();
}

UCookieLandBaseCueActorData* ACookieLandBaseCueActor::GetBaseData()
{
	return BaseData.Get();
}

UCookieLandPiece* ACookieLandBaseCueActor::GetPiece()
{
	if (Action)
	{
		return Action->GetPiece();
	}
	return nullptr;
}