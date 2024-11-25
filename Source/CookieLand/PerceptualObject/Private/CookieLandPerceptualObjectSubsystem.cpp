// Fill out your copyright notice in the Description page of Project Settings.

#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectSubsystem.h"

void UCookieLandPerceptualObjectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCookieLandPerceptualObjectSubsystem::Deinitialize()
{
	PerceptualObjects.Empty();
	AutoId = 0;

	Super::Deinitialize();
}

int UCookieLandPerceptualObjectSubsystem::AddPerceptualObject()
{
	UCookieLandPerceptualObject* PerceptualObject = CreatePerceptualObject();
	return PerceptualObject->Id;
}

void UCookieLandPerceptualObjectSubsystem::RemovePerceptualObject(int Id)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	PerceptualObjects.Remove(PerceptualObject);
}

void UCookieLandPerceptualObjectSubsystem::UpdatePerceptualObjectLocator(int Id, FCookieLandPieceLocator PieceLocator)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	PerceptualObject->PieceLocation = PieceLocator.PieceLocation;
	PerceptualObject->PieceOrientation = PieceLocator.PieceOrientation;

}

UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::CreatePerceptualObject()
{
	UCookieLandPerceptualObject* PerceptualObject = NewObject<UCookieLandPerceptualObject>();
	PerceptualObject->Id = AutoId++;

	PerceptualObjects.Add(PerceptualObject);
}

UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::FindPerceptualObject(int Id)
{
	for (int Index = 0; Index < PerceptualObjects.Num(); ++Index)
	{
		UCookieLandPerceptualObject* PerceptualObject = PerceptualObjects[Index];
		if (PerceptualObject->Id == Id)
		{
			return PerceptualObject;
		}
	}

	return nullptr;
}