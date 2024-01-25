// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PieceLandSystem.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatePieceLandEvent, FName, int);

UCLASS()
class COOKIELAND_API UPieceLandSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void BeginDestroy() override;

	virtual void Deinitialize() override;

public:
	FCreatePieceLandEvent CreatePieceLandEvent;
protected:

	UPROPERTY()
	class UDataTable* LevelLandDataTable; //FLevelLandDataTable

	UPROPERTY()
	class UBasePieceLand* CurLand;
	
public:

	UFUNCTION(BlueprintCallable)
	void CreateLevelLand(FName levelName);

	UFUNCTION(BlueprintCallable)
	UBasePieceLand* GetCurLand();
};