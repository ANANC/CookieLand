// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLandMapSubsystem.generated.h"

class ACookieLandMapBuildActor;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMainMapChangeEvent, FName, MapName);

UCLASS(Config = Game)
class COOKIELAND_API UCookieLandMapSubsystem : public UObject
{
	GENERATED_BODY()

public:
	FMainMapChangeEvent MainMapChangeEvent;

protected:

	UPROPERTY(config, EditDefaultsOnly, meta = (AllowedClasses = "/Script/CookieLand.CookieLandMapBuildActor", DisplayName = "CookieLandGameData"))
	FSoftObjectPath MapBuildActorRef;

	UPROPERTY()
	TArray<ACookieLandMapBuildActor*> MapBuildActors;

	UPROPERTY()
	ACookieLandMapBuildActor* MainMapBuildActor = nullptr;

public:
	void RegisterMapBuildActor(ACookieLandMapBuildActor* InMapBuildActor);

	void UnRegisterMapBuildActor(ACookieLandMapBuildActor* InMapBuildActor);

public:
	UFUNCTION(BlueprintCallable)
	void EnterMap(FName MapName, bool bDestroyLastMap);

	UFUNCTION(BlueprintPure)
	ACookieLandMapBuildActor* GetMainMapBuildActor();

	UFUNCTION(BlueprintPure)
	ACookieLandMapBuildActor* FindMapBuildActor(FName MapName);

protected:
	ACookieLandMapBuildActor* CreateMapBuildActor(FName MapName);
};
