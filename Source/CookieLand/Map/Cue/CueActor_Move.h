// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLandBaseCueActor.h"
#include "CueActor_Move.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API ACueActor_Move : public ACookieLandBaseCueActor
{
	GENERATED_BODY()
	
public:
	virtual void Init(UCookieLandBasePieceAction* InAction, UCookieLandBaseCueActorData* InBaseData) override;
	virtual void UnInit() override;

protected:
	UPROPERTY()
	TObjectPtr<UCookieLandCueData_Move> MoveData;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> StaticMesh_Up;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent>  StaticMesh_Down;

};
