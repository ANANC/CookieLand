// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CookieLandGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UCookieLandGameInstance :  public UGameInstance
{
	GENERATED_BODY()

public:
	UCookieLandGameInstance(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UDataTable* LevelLandDataTable; //FLevelLandDataTable

public:

	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void LoadComplete(const float LoadTime, const FString& MapName) override;
};
