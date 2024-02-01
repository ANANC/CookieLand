// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonFunctionLibrary.h"

#include "CookieLandGameInstance.h"
#include "CookieLand/Piece/PieceLandSystem.h"
#include "GameFramework/Character.h"

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
	float x = abs(direction.X),y = abs(direction.Y),z = abs(direction.Z);

	if(z > x && z > y)
	{
		if(direction.Z>0)
		{
			return EPieceDirection::Up;
		}
		else if(direction.Z<0)
		{
			return EPieceDirection::Down;
		}
	}else if(x > y && x > z)
	{
		if(direction.X>0)
		{
			return EPieceDirection::Right;
		}
		else if(direction.X<0)
		{
			return EPieceDirection::Left;
		}
	}
	else
	{
		if(direction.Y>0)
		{
			return EPieceDirection::Forward;
		}else if(direction.Y<0)
		{
			return EPieceDirection::Backward;
		}
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

FVector UCommonFunctionLibrary::ChangeToControllerDirection(EPieceDirection direction,ACharacter* character)
{
	FQuat quat = character->GetController()->GetControlRotation().Quaternion();
	
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

EPieceDirection UCommonFunctionLibrary::ChangeToLandDirectionByCharacter(EPieceDirection direction,ACharacter* character)
{
	FVector controllerDirection = ChangeToControllerDirection(direction,character);
	EPieceDirection newDirection = ChangeToGameDirection(controllerDirection);
	
	return newDirection;
}

float UCommonFunctionLibrary::GetAngleBetween3DVector(FVector Vec1, FVector Vec2, FVector RefUpVector)
{
	Vec1.Normalize();
	Vec2.Normalize();
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Vec1 | Vec2));
	if (FVector::DotProduct(FVector::CrossProduct(Vec1, Vec2), RefUpVector) < 0)
	{
		Angle = -Angle;
	}

	return Angle;
}

float UCommonFunctionLibrary::GetAngleBetweenVector(FVector Vec1, FVector Vec2)
{
	return GetAngleBetween3DVector(Vec1,Vec2,FVector::UpVector);
}

float UCommonFunctionLibrary::GetForwardAngleBetweenVector(FVector sourceForward,FVector sourceVector, FVector otherVector)
{
	float angle = abs(GetAngleBetweenVector(sourceVector, otherVector));
	float forwardAngle = abs(GetAngleBetweenVector(sourceForward, otherVector));
	
	if (forwardAngle > 90)
	{
		angle = -angle;
	}

	return angle;
}

FVector UCommonFunctionLibrary::ChangeVectorByRotation(float rotation,FVector position)
{
	float radian = 3.14 / 180 * rotation;
	FVector newPos = position;
	newPos.X = position.X * cos(radian) - position.Y * sin(radian);
	newPos.Y = position.X * sin(radian) + position.Y * cos(radian);
	return newPos;
}