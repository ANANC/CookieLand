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

	UFUNCTION(BlueprintPure)
	static FVector ChangeToLogicDirection(EPieceDirection direction);

	UFUNCTION(BlueprintPure)
	static FVector ChangeToActorDirection(EPieceDirection direction,AActor* actor);
	
	UFUNCTION(BlueprintPure)
	static FVector ChangeToControllerDirection(EPieceDirection direction,ACharacter* character);
	
	UFUNCTION(BlueprintPure)
	static EPieceDirection ChangeToLandDirectionByCharacter(EPieceDirection direction,ACharacter* character);
	
	//计算两个向量的夹角
	UFUNCTION(BlueprintPure)
	static float GetAngleBetween3DVector(FVector Vec1, FVector Vec2, FVector RefUpVector);

	UFUNCTION(BlueprintPure)
	static float GetAngleBetweenVector(FVector Vec1, FVector Vec2);

	//计算角度，角度的正负判断基于是否在第一，四象内（就是是否在Forward内）
	UFUNCTION(BlueprintPure)
	static float GetForwardAngleBetweenVector(FVector sourceForward,FVector sourceVector, FVector otherVector);

	//旋转坐标角度
	UFUNCTION(BlueprintPure)
	static FVector ChangeVectorByRotation(float rotation,FVector position);
	

};
