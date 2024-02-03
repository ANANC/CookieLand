// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToComponent.h"

#include "AIReactionAction_BaseMoveTo.h"
#include "AIReactionAction_MoveToPiece.h"
#include "CookieLand/Character/BaseCharacter.h"

// Sets default values for this component's properties
UMoveToComponent::UMoveToComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMoveToComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ABaseCharacter>(GetOwner());
}


// Called every frame
void UMoveToComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(MoveToAction)
	{
		MoveToAction->TickMoveTo(DeltaTime);
		if(MoveToAction->GetExecuteState() != EAIConditionReactionActionExecuteState::Continue)
		{
			StopMoveToAction();
		}
	}
}

void UMoveToComponent::MoveTo(class UAIReactionBaseMoveToActionData* data)
{
	StopMoveToAction();
	StartMoveToAction(data);
}

void UMoveToComponent::StartMoveToAction(class UAIReactionBaseMoveToActionData* data)
{
	if(!data || !data->ActionClass)
	{
		return;
	}

	MoveToAction = NewObject<UAIReactionAction_BaseMoveTo>(this,data->ActionClass);
	if(!MoveToAction)
	{
		return;
	}

	MoveToAction->SetAI(Character);
	MoveToAction->SetActionData(data);
	MoveToAction->Init();

	StartMoveToEvent.Broadcast();
}
	
void UMoveToComponent::StopMoveToAction()
{
	if(!MoveToAction)
	{
		return;
	}

	MoveToAction->UnInit();
	MoveToAction = nullptr;

	FinishMoveToEvent.Broadcast();
}