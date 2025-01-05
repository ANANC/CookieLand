// Fill out your copyright notice in the Description page of Project Settings.


#include "PerceptualObjectInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CookieLand/Map/CookieLandMapBuildLibrary.h"
#include "CookieLandPerceptualObjectComponent.h"

// Sets default values for this component's properties
UPerceptualObjectInputComponent::UPerceptualObjectInputComponent()
{
}


// Called when the game starts
void UPerceptualObjectInputComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = UCookieLandMapBuildLibrary::GetEnhancedInputLocalPlayerSubsystem(this))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}

	EnhancedInputComponent = GetOwner()->GetComponentByClass<UEnhancedInputComponent>();
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,this,&UPerceptualObjectInputComponent::MoveActionTriggerCallback);
	}

	PerceptualObjectComponent = GetOwner()->GetComponentByClass<UCookieLandPerceptualObjectComponent>();
}

void UPerceptualObjectInputComponent::MoveActionTriggerCallback(const FInputActionValue& OutValue)
{
	if (OutValue.GetValueType() == EInputActionValueType::Axis2D)
	{
		ECookieLandPieceOrientation Orientation;

		FVector2D Value = OutValue.Get<FVector2D>();
		if (Value == FVector2D(-1, 0))
		{
			Orientation = ECookieLandPieceOrientation::Left;
		}
		else if(Value == FVector2D(1, 0))
		{
			Orientation = ECookieLandPieceOrientation::Right;
		}
		else if (Value == FVector2D(0, -1))
		{
			Orientation = ECookieLandPieceOrientation::Backward;
		}
		else if (Value == FVector2D(0, 1))
		{
			Orientation = ECookieLandPieceOrientation::Forward;
		}
		else
		{
			return;
		}
		// 地块上下时不允许移动

		//判断是否走到边缘
		// 交给pieceactor进行判断
		{
			// 判断能否转向该方向
			FCookieLandLocation OutLocation;
			if (!GetNextPiece(Orientation, OutLocation))
			{
				return;
			}
		}
	}
}

bool UPerceptualObjectInputComponent::GetNextPiece(ECookieLandPieceOrientation Orientation, FCookieLandLocation& OutLocation)
{
	return false;
}
