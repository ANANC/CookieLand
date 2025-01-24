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
#include "GameFramework/Character.h"
#include "CookieLand/Map/CookieLandMapSubsystem.h"
#include "CookieLand/Character/BasePerceptualObjectController.h"

// Sets default values for this component's properties
UPerceptualObjectInputComponent::UPerceptualObjectInputComponent()
{
}

void UPerceptualObjectInputComponent::SetupInputComponent(UEnhancedInputComponent* InEnhancedInputComponent)
{
	EnhancedInputComponent = InEnhancedInputComponent;
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UPerceptualObjectInputComponent::MoveActionTriggerCallback);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = UCookieLandMapBuildLibrary::GetEnhancedInputLocalPlayerSubsystem(this))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}
}

void UPerceptualObjectInputComponent::SetPawn(APawn* InPawn)
{
	PerceptualObjectComponent = InPawn->GetComponentByClass<UCookieLandPerceptualObjectComponent>();
}

void UPerceptualObjectInputComponent::MoveActionTriggerCallback(const FInputActionValue& OutValue)
{
	if (OutValue.GetValueType() == EInputActionValueType::Axis2D)
	{
		TObjectPtr< ACookieLandMapBuildActor> MapBuildActor = GetMainMapBuildActor();
		if (!MapBuildActor)
		{
			return;
		}

		ECookieLandPieceOrientation Orientation;

		FVector InputDirection = FVector();

		FVector2D Value = OutValue.Get<FVector2D>();
		if (Value == FVector2D(-1, 0))
		{
			InputDirection = -GetController()->GetControlRotation().Quaternion().GetRightVector();
			Orientation = ECookieLandPieceOrientation::Left;
		}
		else if(Value == FVector2D(1, 0))
		{
			InputDirection = GetController()->GetControlRotation().Quaternion().GetRightVector();
			Orientation = ECookieLandPieceOrientation::Right;
		}
		else if (Value == FVector2D(0, -1))
		{
			InputDirection = -GetController()->GetControlRotation().Quaternion().GetForwardVector();
			Orientation = ECookieLandPieceOrientation::Backward;
		}
		else if (Value == FVector2D(0, 1))
		{
			InputDirection = GetController()->GetControlRotation().Quaternion().GetForwardVector();
			Orientation = ECookieLandPieceOrientation::Forward;
		}
		else
		{
			return;
		}


		FCookieLandPieceLocator CurrentLocator = PerceptualObjectComponent->GetLocator();

		UCookieLandMapActorGather* MapActorGather = MapBuildActor->GetMapActorGather();
		UCookieLandPiece* CurrentPiece = MapActorGather->GetPiece(CurrentLocator.PieceLocation, CurrentLocator.PieceOrientation);
		if (!CurrentPiece || !CurrentPiece->GetPieceActor())
		{
			return;
		}

		// 地块上下时不允许移动
		if (UCookieLandPieceLibrary::HasActiveGameplayTag_Moving(CurrentPiece))
		{
			return;
		}

		//判断是否走到边缘
		if(CurrentPiece->GetPieceAction()->GetOnEdge())
		{
			// 判断能否转向该方向
			FCookieLandPieceLocator OutLocation;
			if (!GetNextPiece(Orientation, OutLocation))
			{
				return;
			}
		}

		// 移动
		GetCharacter()->AddMovementInput(InputDirection, 1.0, false);
	}
}

bool UPerceptualObjectInputComponent::GetNextPiece(ECookieLandPieceOrientation Orientation, FCookieLandPieceLocator& OutLocator)
{
	TObjectPtr< ACookieLandMapBuildActor> MapBuildActor = GetMainMapBuildActor();
	if (!MapBuildActor)
	{
		return false;
	}

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

TObjectPtr<class ACookieLandMapBuildActor> UPerceptualObjectInputComponent::GetMainMapBuildActor()
{
	TObjectPtr< ACookieLandMapBuildActor> MapBuildActor = UCookieLandMapBuildLibrary::GetMapSubsystem()->GetMainMapBuildActor();
	if (MapBuildActor)
	{
		return MapBuildActor;
	}
	return nullptr;
}

ABasePerceptualObjectController* UPerceptualObjectInputComponent::GetController()
{
	return Cast<ABasePerceptualObjectController>(GetOwner());
}

ACharacter* UPerceptualObjectInputComponent::GetCharacter()
{
	return Cast<ACharacter>(GetController()->GetPawn());
}