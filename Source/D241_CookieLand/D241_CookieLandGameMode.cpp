// Copyright Epic Games, Inc. All Rights Reserved.

#include "D241_CookieLandGameMode.h"
#include "D241_CookieLandPlayerController.h"
#include "D241_CookieLandCharacter.h"
#include "UObject/ConstructorHelpers.h"

AD241_CookieLandGameMode::AD241_CookieLandGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AD241_CookieLandPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}