// Fill out your copyright notice in the Description page of Project Settings.

#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectSubsystem.h"


int UCookieLandPerceptualObjectSubsystem::AddPerceptualObject(bool bMainPerceptualObject, FName InPerceptualObjectType, bool bInEnablePerceptual)
{
	UCookieLandPerceptualObject* PerceptualObject = CreatePerceptualObject();
	PerceptualObject->PerceptualObjectType = InPerceptualObjectType;
	PerceptualObject->bEnablePerceptual = bInEnablePerceptual;

	if (bMainPerceptualObject)
	{
		MainPerceptualObject = PerceptualObject;
	}

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

	UpdatePassivePerceptualObjectLocators();
}

void UCookieLandPerceptualObjectSubsystem::UpdatePerceptualObjectLocator(int Id, FCookieLandPieceLocator PieceLocator)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	FCookieLandPieceLocator OldPieceLocator = FCookieLandPieceLocator(PerceptualObject->PieceLocation, PerceptualObject->PieceOrientation);

	PerceptualObject->PieceLocation = PieceLocator.PieceLocation;
	PerceptualObject->PieceOrientation = PieceLocator.PieceOrientation;

	UpdatePassivePerceptualObjectLocators();

	if (PerceptualObject == MainPerceptualObject)
	{
		MainPerceptualObjectLocatorChangeEvent.Broadcast(PerceptualObject->Id, OldPieceLocator, PieceLocator);
	}
	PerceptualObjectLocatorChangeEvent.Broadcast(PerceptualObject->Id, OldPieceLocator, PieceLocator);
}

void UCookieLandPerceptualObjectSubsystem::UpdatePerceptualObjectEnablePerceptual(int Id, bool bInEnablePerceptual)
{
	UCookieLandPerceptualObject* PerceptualObject = FindPerceptualObject(Id);
	if (!PerceptualObject)
	{
		return;
	}

	PerceptualObject->bEnablePerceptual = bInEnablePerceptual;

	UpdatePassivePerceptualObjectLocators();
}

UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::CreatePerceptualObject()
{
	UCookieLandPerceptualObject* PerceptualObject = NewObject<UCookieLandPerceptualObject>();
	PerceptualObject->Id = AutoId++;

	PerceptualObjects.Add(PerceptualObject);

	return PerceptualObject;
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

const UCookieLandPerceptualObject* UCookieLandPerceptualObjectSubsystem::GetMainPerceptualObject()
{
	return MainPerceptualObject;
}


bool UCookieLandPerceptualObjectSubsystem::GetMainCurrentLocator(FCookieLandPieceLocator& MainLocator)
{
	if (MainPerceptualObject)
	{
		MainLocator = FCookieLandPieceLocator(MainPerceptualObject->PieceLocation, MainPerceptualObject->PieceOrientation);
		return true;
	}
	return false;
}

void UCookieLandPerceptualObjectSubsystem::SetMapAngleViewType(ECookieLandMapAngleViewType InMapAngleViewType)
{
	MapAngleViewType = InMapAngleViewType;
}

ECookieLandMapAngleViewType UCookieLandPerceptualObjectSubsystem::GetMapAngleViewType()
{
	return MapAngleViewType;
}
void UCookieLandPerceptualObjectSubsystem::UpdatePassivePerceptualObjectLocators()
{
	PassivePerceptualObjectLocators.Empty();

	for (int Index = 0; Index < PerceptualObjects.Num(); ++Index)
	{
		UCookieLandPerceptualObject* PerceptualObject = PerceptualObjects[Index];
		if (PerceptualObject->bEnablePerceptual && PerceptualObject->Id!= MainPerceptualObject->Id)
		{
			PassivePerceptualObjectLocators.Add(FCookieLandPieceLocator(PerceptualObject->PieceLocation, PerceptualObject->PieceOrientation));
		}
	}
}