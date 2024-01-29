// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "CommonFunctionLibrary.generated.h"

class UCookieLandGameInstance;
class UPieceLandSystem;
class UBasePieceLand;

UCLASS()
class COOKIELAND_API UCommonFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static UCookieLandGameInstance* GetGameInstance(class UWorld* world);
	
	UFUNCTION(BlueprintPure)
	static UPieceLandSystem* GetPieceLandSystem();
	
	UFUNCTION(BlueprintPure)
	static UBasePieceLand* GetCurPieceLand();

	UFUNCTION(BlueprintPure)
	static FVector ChangeToLogicLocation(FPieceLocation location);
	
	UFUNCTION(BlueprintPure)
	static FPieceLocation ChangeToGameLocation(FVector location);

	UFUNCTION(BlueprintPure)
	static EPieceDirection ChangeToGameDirection(FVector direction);
};
