// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonFunctionLibrary.h"

#include "CookieLandGameInstance.h"
#include "CookieLand/Piece/PieceLandSystem.h"

UCookieLandGameInstance* UCommonFunctionLibrary::GetGameInstance(class UWorld* world)
{
	UCookieLandGameInstance* GameInstance = nullptr;
	
	if (world)
	{
		UGameInstance* instance = world->GetGameInstance();
		if(instance)
		{
			GameInstance = Cast<UCookieLandGameInstance>(instance);
		}
	}

	return GameInstance;
}

UPieceLandSystem* UCommonFunctionLibrary::GetPieceLandSystem()
{
	if(!GWorld)
	{
		return nullptr;
	}
	UCookieLandGameInstance* instance = GetGameInstance(GWorld->GetWorld());
	if(instance)
	{
		return instance->GetSubsystem<UPieceLandSystem>();
	}
	return nullptr;
}

UBasePieceLand* UCommonFunctionLibrary::GetCurPieceLand()
{
	UPieceLandSystem* pieceLandSystem = GetPieceLandSystem();
	if(pieceLandSystem)
	{
		return pieceLandSystem->GetCurLand();
	}

	return nullptr;
}

FVector UCommonFunctionLibrary::ChangeToLogicLocation(FPieceLocation location)
{
	FVector logicLocation = FVector(location.X,location.Y,location.Floor);
	return logicLocation;
}
	
FPieceLocation UCommonFunctionLibrary::ChangeToGameLocation(FVector location)
{
	FPieceLocation gameLocation(location);
	return gameLocation;
}

EPieceDirection UCommonFunctionLibrary::ChangeToGameDirection(FVector direction)
{
	if(direction.Z>0)
	{
		return EPieceDirection::Up;
	}
	else if(direction.Z<0)
	{
		return EPieceDirection::Down;
	}
	else if(direction.X>0)
	{
		return EPieceDirection::Right;
	}
	else if(direction.X<0)
	{
		return EPieceDirection::Left;
	}
	else if(direction.Y>0)
	{
		return EPieceDirection::Forward;
	}else if(direction.Y<0)
	{
		return EPieceDirection::Backward;
	}
	return  EPieceDirection::Forward;
}