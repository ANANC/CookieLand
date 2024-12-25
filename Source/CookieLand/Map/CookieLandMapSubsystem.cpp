// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapSubsystem.h"
#include "CookieLandMapBuildActor.h"

void UCookieLandMapSubsystem::RegisterMapBuildActor(ACookieLandMapBuildActor* InMapBuildActor)
{
	if (!InMapBuildActor)
	{
		return;
	}
	MapBuildActors.AddUnique(InMapBuildActor);
}

void UCookieLandMapSubsystem::UnRegisterMapBuildActor(ACookieLandMapBuildActor* InMapBuildActor)
{
	if (!InMapBuildActor)
	{
		return;
	}
	MapBuildActors.Remove(InMapBuildActor);
}

void UCookieLandMapSubsystem::EnterMap(FName MapName,bool bDestroyLastMap)
{
	if (MainMapBuildActor)
	{
		if (MainMapBuildActor->MapName == MapName)
		{
			return;
		}

		MainMapBuildActor->ExistThisMap(bDestroyLastMap);
	}

	ACookieLandMapBuildActor* MapBuildActor = FindMapBuildActor(MapName);
	if (!MapBuildActor)
	{
		MapBuildActor = CreateMapBuildActor(MapName);
	}

	MapBuildActor->CreateEnvironment();
	MainMapBuildActor = MapBuildActor;

	MainMapChangeEvent.Broadcast(MapName);
}

ACookieLandMapBuildActor* UCookieLandMapSubsystem::CreateMapBuildActor(FName MapName)
{
	FTransform InitTransfrom;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UObject* MapBuildActorRefObject = MapBuildActorRef.TryLoad();

	AActor* InstanceActor = GetWorld()->SpawnActor(MapBuildActorRefObject->GetClass(), &InitTransfrom, SpawnParameters);
	return Cast< ACookieLandMapBuildActor>(InstanceActor);
}

ACookieLandMapBuildActor* UCookieLandMapSubsystem::GetMainMapBuildActor()
{
	return MainMapBuildActor;
}

ACookieLandMapBuildActor* UCookieLandMapSubsystem::FindMapBuildActor(FName MapName)
{
	for (int Index = 0; Index < MapBuildActors.Num(); ++Index)
	{
		ACookieLandMapBuildActor* MapBuildActor = MapBuildActors[Index];
		if (MapBuildActor && MapBuildActor->MapName == MapName)
		{
			return MapBuildActor;
		}
	}

	return nullptr;
}