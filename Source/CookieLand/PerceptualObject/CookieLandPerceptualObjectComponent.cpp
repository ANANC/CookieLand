// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandPerceptualObjectComponent.h"
#include "CookieLandPerceptualObjectSubsystem.h"
#include "CookieLand/Map/CookieLandMapBuildLibrary.h"

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
	FCookieLandPerceptualObjectPerceptionInfo PerceptionInfo;
	UCookieLandMapBuildLibrary::GetPerceptualObjectPerceptionInfoFromTable(PerceptualObjectType, PerceptionInfo);

	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
	if (PerceptualObjectSubsystem)
	{
		PerceptualObjectId = PerceptualObjectSubsystem->AddPerceptualObject(bMainPerceptualObject, PerceptualObjectType, true);
	}

	SetLocator(InitLocator);
}

void UCookieLandPerceptualObjectComponent::UnInit()
{
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
	if (PerceptualObjectSubsystem)
	{
		PerceptualObjectSubsystem->RemovePerceptualObject(PerceptualObjectId);
		PerceptualObjectId = -1;
	}
}

int UCookieLandPerceptualObjectComponent::GetPerceptualObjectId() const
{
	return PerceptualObjectId;
}

const FCookieLandPerceptualObjectPerceptionInfo UCookieLandPerceptualObjectComponent::GetPerceptionInfo() const
{
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
	if (PerceptualObjectSubsystem)
	{
		const UCookieLandPerceptualObject* UCookieLandPerceptualObject = PerceptualObjectSubsystem->GetPerceptualObject(PerceptualObjectId);
		if (UCookieLandPerceptualObject)
		{
			return UCookieLandPerceptualObject->PerceptionInfo;
		}
	}
	FCookieLandPerceptualObjectPerceptionInfo PerceptionInfo;
	return PerceptionInfo;
}

void UCookieLandPerceptualObjectComponent::SetLocation(FCookieLandLocation InMapCubeLocation)
{
	SetLocator(FCookieLandPieceLocator(InMapCubeLocation, PieceOrientation));
}

void UCookieLandPerceptualObjectComponent::SetLocator(FCookieLandPieceLocator InLocator)
{
	MapCubeLocation = InLocator.PieceLocation;
	PieceOrientation = InLocator.PieceOrientation;

	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
	if (PerceptualObjectSubsystem)
	{
		PerceptualObjectSubsystem->UpdatePerceptualObjectLocator(PerceptualObjectId, InLocator);
	}
}