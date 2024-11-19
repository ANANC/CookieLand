// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandGlobal.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCookieLandGlobal, Log, All);

UCLASS(BlueprintType, Config = Game)
class UCookieLandGameData : public UDataAsset
{
	GENERATED_BODY()

public:
	static const UCookieLandGameData& Get();

	UCookieLandGameData();

public:
	void LoadStartupData();
	void ClearCacheData();

public:
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, meta = (DisplayName = "地图Table"))
	class UDataTable* MapBuildDataTable;//FCookieLandMapBuildDataTableRow
};

UCLASS(config = Engine)
class COOKIELAND_API UCookieLandGlobal : public UObject
{
	GENERATED_BODY()
	
public:
	static FORCEINLINE UCookieLandGlobal& Get()
	{
		return *static_cast<UCookieLandGlobal*>(GEngine->GameSingleton);
	}

	FORCEINLINE_DEBUGGABLE UCookieLandGameData const& GetGameData()
	{
		if (!GameData)
		{
			LoadGameData();
			check(GameData);
		}
		return *GameData;
	}

	FORCEINLINE_DEBUGGABLE UCookieLandGameData* GetGameDataPtr()
	{
		if (!GameData)
		{
			LoadGameData();
			check(GameData);
		}
		return GameData;
	}

	static void ReloadGameData();

protected:
	UPROPERTY(config, EditDefaultsOnly, meta = (AllowedClasses = "/Script/CookieLand.CookieLandGameData", DisplayName = "CookieLandGameData"))
	FSoftObjectPath GameDataRef;

	UPROPERTY()
	UCookieLandGameData* GameData;

private:
	void LoadGameData();
	void ClearGameData();
};
