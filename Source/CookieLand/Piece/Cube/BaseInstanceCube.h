// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "UObject/Object.h"
#include "BaseInstanceCube.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UBaseInstanceCube : public UObject
{
	GENERATED_BODY()

protected:
	int CubeId;
	
	UPROPERTY()
	class UBasePieceLand* OwnLand;
	
	UPROPERTY()
	class UInstanceCubeVolume* CubeVolume;
	
public:

	void SetCubeId(int id);

	void SetOwnLand(class UBasePieceLand* land);
	
	void Init();

	void UnInit();

	UFUNCTION(BlueprintPure)
	int GetCubeId();
	
	UFUNCTION(BlueprintPure)
	class UBasePieceLand* GetOwnLand();

public:
	UFUNCTION(BlueprintPure)
	bool CreateCube(FPieceLocation location);
	
	UFUNCTION(BlueprintPure)
	bool AddVolume(FPieceLocation location);
	
	UFUNCTION(BlueprintPure)
	bool GetIsInSide(FPieceLocation location);
	
	UFUNCTION(BlueprintPure)
	bool GetIsInEdge(FPieceLocation location);
};
