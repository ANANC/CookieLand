// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveToComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveToStateEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOKIELAND_API UMoveToComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveToComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FMoveToStateEvent StartMoveToEvent;
	
	UPROPERTY(BlueprintAssignable)
	FMoveToStateEvent FinishMoveToEvent;

protected:

	UPROPERTY(BlueprintReadOnly)
	class ABaseCharacter* Character;
	
	UPROPERTY()
	class UAIReactionAction_BaseMoveTo* MoveToAction;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void MoveTo(class UAIReactionBaseMoveToActionData* data);

protected:

	void StartMoveToAction(class UAIReactionBaseMoveToActionData* data);
	
	void StopMoveToAction();
};
