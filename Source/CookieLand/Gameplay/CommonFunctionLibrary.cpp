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

FVector UCommonFunctionLibrary::ChangeToLogicDirection(EPieceDirection direction)
{
	switch (direction)
	{
	case EPieceDirection::Left:
		return FVector::LeftVector;
	case EPieceDirection::Right:
		return FVector::RightVector;
	case EPieceDirection::Forward:
		return FVector::ForwardVector;
	case EPieceDirection::Backward:
		return FVector::BackwardVector;
	case EPieceDirection::Up:
		return FVector::UpVector;
	case EPieceDirection::Down:
		return FVector::DownVector;
	default:
		return FVector::LeftVector;
	}
}

FVector UCommonFunctionLibrary::ChangeToActorDirection(EPieceDirection direction,AActor* actor)
{
	FQuat quat = actor->GetActorRotation().Quaternion();
	
	switch (direction)
	{
	case EPieceDirection::Left:
		return -quat.GetRightVector();
	case EPieceDirection::Right:
		return quat.GetRightVector();
	case EPieceDirection::Forward:
		return quat.GetForwardVector();
	case EPieceDirection::Backward:
		return -quat.GetForwardVector();
	case EPieceDirection::Up:
		return quat.GetUpVector();
	case EPieceDirection::Down:
		return -quat.GetUpVector();
	default:
		return quat.GetForwardVector();
	}
}