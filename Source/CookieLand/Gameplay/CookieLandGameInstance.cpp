// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandGameInstance.h"

UCookieLandGameInstance::UCookieLandGameInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

void UCookieLandGameInstance::Init()
{
	Super::Init();
}

void UCookieLandGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UCookieLandGameInstance::LoadComplete(const float LoadTime, const FString& MapName)
{
	Super::LoadComplete(LoadTime, MapName);
}