// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonFunctionLibrary.h"

#include "CookieLandGameInstance.h"
#include "CookieLand/Piece/PieceLandSystem.h"

UCookieLandGameInstance* UCommonFunctionLibrary::GetGameInstance(class UWorld* world)
{
	UCookieLandGameInstance* GameInstance = nullptr;
	
	if (world)
	{
		UGameInstance* instance = world->GetGameInstance();
		if(instance)
		{
			GameInstance = Cast<UCookieLandGameInstance>(instance);
		}
	}

	return GameInstance;
}

UPieceLandSystem* UCommonFunctionLibrary::GetPieceLandSystem()
{
	if(!GWorld)
	{
		return nullptr;
	}
	UCookieLandGameInstance* instance = GetGameInstance(GWorld->GetWorld());
	if(instance)
	{
		return instance->GetSubsystem<UPieceLandSystem>();
	}
	return nullptr;
}

UBasePieceLand* UCommonFunctionLibrary::GetCurPieceLand()
{
	UPieceLandSystem* pieceLandSystem = GetPieceLandSystem();
	if(pieceLandSystem)
	{
		return pieceLandSystem->GetCurLand();
	}

	return nullptr;
}