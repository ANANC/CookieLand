// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceCardComponent.h"


// Sets default values for this component's properties
UPieceCardComponent::UPieceCardComponent()
{
	// ...
}


// Called when the game starts
void UPieceCardComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UPieceCardComponent::IsEnableUseCard(FName cardName)
{
	if(IsEnableUseAllCard)
	{
		return true;
	}

	for(int index = 0;index<EnableCardNames.Num();++index)
	{
		if(EnableCardNames[index] == cardName)
		{
			return true;
		}
	}

	return false;
}