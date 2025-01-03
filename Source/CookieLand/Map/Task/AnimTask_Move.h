// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Map/CookieLandBaseAnimTask.h"
#include "CookieLand/Map/CookieLandActionTypes.h"
#include "AnimTask_Move.generated.h"

// 基础Task：移动
UCLASS()
class COOKIELAND_API UAnimTask_Move : public UCookieLandBaseAnimTask
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
	FCookieLandPieceLocator StartLocation;
	UPROPERTY()
	FCookieLandPieceLocator EndLocation;

	UPROPERTY()
	FAnimTaskMoveData MoveData;

public:
	static UAnimTask_Move* CreateTask(UCookieLandBasePieceAction* ThisAction, const FCookieLandPieceLocator& InStartLocation, const FCookieLandPieceLocator& InEndLocation, const FAnimTaskMoveData& InData);

public:
	virtual void Active() override;

protected:
	// 播放timeline
	void PlayTimeline();

	UFUNCTION()
	void UpdateTimeline(float Value);

	UFUNCTION()
	void OnTimelineFinished();
};
