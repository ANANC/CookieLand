// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonFunctionLibrary.h"

#include "CookieLandGameInstance.h"
#include "CookieLand/Character/BaseCharacter.h"
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

class ABaseCharacter* UCommonFunctionLibrary::GetMainCharacter(class UWorld* world)
{
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			APawn* pawn = playerController->GetPawn();
			if (pawn)
			{
				ABaseCharacter* player = Cast< ABaseCharacter>(pawn);
				if (player)
				{
					return player;
				}
			}
		}
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
		return FVector(-1,0,0);
	case EPieceDirection::Right:
		return  FVector(1,0,0);
	case EPieceDirection::Forward:
		return  FVector(0,1,0);
	case EPieceDirection::Backward:
		return FVector(0,-1,0);
	case EPieceDirection::Up:
		return FVector(0,0,1);
	case EPieceDirection::Down:
		return FVector(0,0,-1);
	default:
		return FVector(-1,0,0);
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
	float yaw = character->GetController()->GetControlRotation().Yaw;

	FVector characterForward = ChangeVectorByAngle(90,ChangeToLogicDirection(EPieceDirection::Forward));
	FVector controllerForward = ChangeVectorByAngle(yaw,characterForward);
	
	float angle = GetAngleBetweenVector(ChangeToLogicDirection(direction),ChangeToLogicDirection(EPieceDirection::Forward));
	FVector inputVector = ChangeVectorByAngle(angle,controllerForward);
	EPieceDirection newDirection = ChangeToGameDirection(inputVector);
	
	return newDirection;
}

EPieceDirection UCommonFunctionLibrary::ChangeGameDirectionByAngle(float angle,EPieceDirection direction)
{
	FVector dirVector = ChangeToLogicDirection(direction);
	FVector changeVector = ChangeVectorByAngle(angle,dirVector);
	EPieceDirection newDirection = ChangeToGameDirection(changeVector);
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

FVector UCommonFunctionLibrary::ChangeVectorByAngle(float angle,FVector vector)
{
	FVector newVector = vector.RotateAngleAxis(angle,FVector::DownVector);
	return newVector;
}


float UCommonFunctionLibrary::GetSmoothRotationYaw(float curYaw,float targetYaw)
{
    float newTargetYaw = targetYaw;
    if(((curYaw > 90 && curYaw < 180 )|| (curYaw < -90 && curYaw > -180 )) && ((targetYaw > 90 && targetYaw < 180 )|| (targetYaw < -90 && targetYaw > -180 )) && (curYaw * targetYaw) < 0)
    {
	    float angle = (180 - abs(targetYaw)) + (180 - abs(curYaw));
    	
    	if(targetYaw < curYaw)//right
    	{
    		newTargetYaw = curYaw + angle;
    	}
    	else
    	{
    		newTargetYaw = curYaw - angle;
    	}
    }
    

    return newTargetYaw;
}

float UCommonFunctionLibrary::ChangeToControllerYaw(float curYaw)
{
    float newYaw = curYaw;
    if(curYaw > -360 && curYaw < -180)
    {
       newYaw = 360+curYaw;
    }
    else if(curYaw < 360 && curYaw > 180)
    {
       newYaw = curYaw-360;
    }

    return newYaw;
}

float UCommonFunctionLibrary::GetDifferenceValueByControllerYaw(float curYaw,float targetYaw)
{
    int moveDirection = curYaw > targetYaw? -1:1;
    float value;
    if(curYaw*targetYaw>0)
    {
       value = abs(curYaw - targetYaw);
    }
    else    {
       float positiveValue = curYaw>=0?curYaw:targetYaw;
       float minusValue = curYaw<0?curYaw:targetYaw;

       if(positiveValue + abs(minusValue) < 180)
       {
          value = positiveValue + abs(minusValue);
       }
       else       {
          value = (180-positiveValue) + abs(-180-minusValue);
          moveDirection *= -1;
       }
    }

    value *= moveDirection;
    return value;
}