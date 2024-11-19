// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Global/Public/CookieLandGlobal.h"

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