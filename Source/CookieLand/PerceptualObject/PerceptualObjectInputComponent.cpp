// Fill out your copyright notice in the Description page of Project Settings.


#include "PerceptualObjectInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CookieLand/Map/CookieLandMapBuildLibrary.h"
#include "CookieLandPerceptualObjectComponent.h"
#include "CookieLand/Map/CookieLandMapBuildActor.h"
#include "CookieLand/Map/CookieLandMapBuilder.h"
#include "CookieLand/Map/CookieLandPiece.h"
#include "CookieLand/Map/CookieLandMapActorGather.h"

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

void UPerceptualObjectInputComponent::Init(ACookieLandMapBuildActor* InMapBuildActor)
{
	MapBuildActor = InMapBuildActor;
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

		FCookieLandPieceLocator CurrentLocator = PerceptualObjectComponent->GetLocator();

		UCookieLandMapActorGather* MapActorGather = MapBuildActor->GetMapActorGather();
		UCookieLandPiece* CurrentPiece = MapActorGather->GetPiece(CurrentLocator.PieceLocation, CurrentLocator.PieceOrientation);
		if (!CurrentPiece)
		{
			return;
		}

		// 地块上下时不允许移动
		if (UCookieLandPieceLibrary::HasActiveGameplayTag_Moving(CurrentPiece))
		{
			return;
		}


		//判断是否走到边缘
		// 交给pieceactor进行判断
		{
			// 判断能否转向该方向
			FCookieLandPieceLocator OutLocation;
			if (!GetNextPiece(Orientation, OutLocation))
			{
				return;
			}
		}

		// 移动
		/*FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);*/
	}
}

bool UPerceptualObjectInputComponent::GetNextPiece(ECookieLandPieceOrientation Orientation, FCookieLandPieceLocator& OutLocator)
{
	if (!PerceptualObjectComponent)
	{
		return false;
	}

	OutLocator = PerceptualObjectComponent->GetLocator();
	OutLocator.PieceLocation.AddDistanceBySixDirection(Orientation, 1);

	UCookieLandMapBuilder* Builder = MapBuildActor->GetMapBuilder();

	if (Builder->GetIsPieceOccupyByLocator(OutLocator.PieceLocation, OutLocator.PieceOrientation))
	{
		return true;
	}

	return false;
}
