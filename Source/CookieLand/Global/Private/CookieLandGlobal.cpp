// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Global/Public/CookieLandGlobal.h"
#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectSubsystem.h"
#include "CookieLand/Map/Public/CookieLandMapSubsystem.h"

DEFINE_LOG_CATEGORY(LogCookieLandGlobal);

static FAutoConsoleCommand CVarReloadGameData(
	TEXT("CookieLandGlobal.ReloadGameData"),
	TEXT("Reload Game Data"),
	FConsoleCommandDelegate::CreateStatic(UCookieLandGlobal::ReloadGameData),
	ECVF_Cheat);

const UCookieLandGameData& UCookieLandGameData::Get()
{
	return UCookieLandGlobal::Get().GetGameData();
}

UCookieLandGameData::UCookieLandGameData() 
{
}

void UCookieLandGameData::LoadStartupData()
{

}

void UCookieLandGameData::ClearCacheData()
{

}

UCookieLandGlobal::UCookieLandGlobal()
{
	InitSubsystems();
}

void UCookieLandGlobal::ReloadGameData()
{
	UCookieLandGlobal& CookieLandGlobal = UCookieLandGlobal::Get();
	CookieLandGlobal.ClearGameData();
	CookieLandGlobal.LoadGameData();
}

void UCookieLandGlobal::LoadGameData()
{
	if (GIsInitialLoad)
	{
		return;
	}

	const FSoftObjectPath* GameDataRefToUse = &GameDataRef;
	if (GameDataRefToUse->ToString().Len() > 0 && !GameData)
	{
		GameData = LoadObject<UCookieLandGameData>(NULL, *GameDataRefToUse->ToString(), NULL, LOAD_None, NULL);
	}
	if (!GameData)
	{
		// None in ini, so build a default
		GameData = NewObject<UCookieLandGameData>(UCookieLandGameData::StaticClass());
	}
	check(GameData);

	UE_LOG(LogCookieLandGlobal, Log, TEXT("Load GameData Success! %s"), *GameDataRefToUse->ToString());

	GameData->LoadStartupData();
}

void UCookieLandGlobal::ClearGameData()
{
	if (GameData)
	{
		GameData->ClearCacheData();
		GameData = nullptr;
	}
}


void UCookieLandGlobal::InitSubsystems()
{
	Subsystems.Add(NewObject<UCookieLandPerceptualObjectSubsystem>());
	Subsystems.Add(NewObject<UCookieLandMapSubsystem>());
}

UObject* UCookieLandGlobal::FindSubsystem(UClass* SubsystemClass)
{
	for (int Index = 0; Index < Subsystems.Num(); ++Index)
	{
		UObject* Subsystem = Subsystems[Index];
		if (Subsystem->GetClass() == SubsystemClass)
		{
			return Subsystem;
		}
	}

	return nullptr;
}