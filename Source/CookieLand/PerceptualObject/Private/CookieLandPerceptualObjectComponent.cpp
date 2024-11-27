// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectComponent.h"
#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectSubsystem.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"

// Sets default values for this component's properties
UCookieLandPerceptualObjectComponent::UCookieLandPerceptualObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCookieLandPerceptualObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
//void UCookieLandPerceptualObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UCookieLandPerceptualObjectComponent::Init(FCookieLandPieceLocator InitLocator)
{
	UCookieLandMapBuildLibrary::GetPerceptualObjectPerceptionInfoFromTable(PerceptualObjectType, PerceptionInfo);

	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem(this);
	if (PerceptualObjectSubsystem)
	{
		PerceptualObjectId = PerceptualObjectSubsystem->AddPerceptualObject(bMainPerceptualObject, PerceptualObjectType, true);
	}

	SetLocator(InitLocator);
}

void UCookieLandPerceptualObjectComponent::UnInit()
{
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem(this);
	if (PerceptualObjectSubsystem)
	{
		PerceptualObjectSubsystem->RemovePerceptualObject(PerceptualObjectId);
		PerceptualObjectId = -1;
	}
}

void UCookieLandPerceptualObjectComponent::SetLocation(FCookieLandLocation InMapCubeLocation)
{
	SetLocator(FCookieLandPieceLocator(InMapCubeLocation, PieceOrientation));
}

void UCookieLandPerceptualObjectComponent::SetLocator(FCookieLandPieceLocator InLocator)
{
	MapCubeLocation = InLocator.PieceLocation;
	PieceOrientation = InLocator.PieceOrientation;

	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem(this);
	if (PerceptualObjectSubsystem)
	{
		PerceptualObjectSubsystem->UpdatePerceptualObjectLocator(PerceptualObjectId, InLocator);
	}
}