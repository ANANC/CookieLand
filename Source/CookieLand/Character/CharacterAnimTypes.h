// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterAnimTypes.generated.h"

UENUM(BlueprintType)
enum class ELSGait : uint8
{
	Walking   UMETA(DisplayName = "Walking"),
	Running   UMETA(DisplayName = "Running"),
	Sprinting UMETA(DisplayName = "Sprinting"),
	Max UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ELSMovementMode : uint8
{
	None     UMETA(DisplayName = "None"),
	Grounded UMETA(DisplayName = "Grounded"),
	Falling  UMETA(DisplayName = "Falling"),
};

UENUM(BlueprintType)
enum class EJumpSequenceStateType : uint8
{
	None		UMETA(DisplayName = "None"),
	JumpUp		UMETA(DisplayName = "JumpUp"),
	JumpDown	UMETA(DisplayName = "JumpDown"),
	//Landing		UMETA(DisplayName = "Landing"),
};
