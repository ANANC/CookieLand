// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimTask_Move.h"
#include "CookieLand/Map/CookieLandPiece.h"
#include "CookieLand/Map/CookieLandMapBuildActor.h"
#include "Components/TimelineComponent.h"
#include "CookieLand/Map/CookieLandMapBuildLibrary.h"

UAnimTask_Move* UAnimTask_Move::CreateTask(UCookieLandBasePieceAction* ThisAction, const FCookieLandPieceLocator& InStartLocation, const FCookieLandPieceLocator& InEndLocation, const FAnimTaskMoveData& InData)
{
	UAnimTask_Move* Task = NewAnimTask< UAnimTask_Move>(ThisAction);
	Task->StartLocation = InStartLocation;
	Task->EndLocation = InEndLocation;
	Task->MoveData = InData;

	Task->Active();

	return Task;
}

void UAnimTask_Move::Active()
{
	Super::Active();

	if (StartLocation == EndLocation || !MoveData.MoveCurvel)
	{
		Finish();
		return;
	}

	PlayTimeline();
}

void UAnimTask_Move::PlayTimeline()
{
	ACookieLandPieceActor* PieceActor = GetPieceActor();
	UTimelineComponent* TimelineComponent = PieceActor->GetTimelineComponent();

	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindDynamic(this,&UAnimTask_Move::UpdateTimeline);
	TimelineComponent->AddInterpFloat(MoveData.MoveCurvel, TimelineCallback);

	FOnTimelineEvent TimelineFinishedCallback;
	TimelineFinishedCallback.BindDynamic(this, &UAnimTask_Move::OnTimelineFinished);
	TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);

	TimelineComponent->SetTimelineLength(MoveData.Duration);
	TimelineComponent->PlayFromStart();
}

void UAnimTask_Move::UpdateTimeline(float Value)
{
	FVector ActorStartLocation = UCookieLandMapBuildLibrary::CalculatePieceActorInstanceLocation(Piece->GetMapBuildActor()->MapBuildInfo, StartLocation.PieceLocation, StartLocation.PieceOrientation);
	FVector ActorEndLocation = UCookieLandMapBuildLibrary::CalculatePieceActorInstanceLocation(Piece->GetMapBuildActor()->MapBuildInfo, EndLocation.PieceLocation, EndLocation.PieceOrientation);

	FVector CurrentLocation = FMath::Lerp(ActorStartLocation, ActorEndLocation, Value);

	ACookieLandPieceActor* PieceActor = GetPieceActor();
	PieceActor->SetActorLocation(CurrentLocation);
}

void UAnimTask_Move::OnTimelineFinished()
{
	Finish();
}