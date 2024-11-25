// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
#include "CookieLandPerceptualObjectSubsystem.generated.h"

UCLASS()
class UCookieLandPerceptualObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Id = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEnablePerceptual = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation PieceOrientation;
};


UCLASS()
class COOKIELAND_API UCookieLandPerceptualObjectSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:

	UPROPERTY()
	TArray< UCookieLandPerceptualObject*> PerceptualObjects;

	int AutoId = 0;

public:
	int AddPerceptualObject();

	void RemovePerceptualObject(int Id);

	void UpdatePerceptualObjectLocator(int Id,FCookieLandPieceLocator PieceLocator);

protected:
	UCookieLandPerceptualObject* CreatePerceptualObject();

	UCookieLandPerceptualObject* FindPerceptualObject(int Id);
};
