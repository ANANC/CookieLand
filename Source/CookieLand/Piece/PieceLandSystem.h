// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PieceTypes.h"
#include "UObject/Object.h"
#include "PieceLandSystem.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatePieceLandEvent, FName, int);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPieceLandShowTipEvent, FPieceTipData,tipData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPieceLandHideTipEvent, bool, allTip, bool, isFixType,int, tipId);

UCLASS()
class COOKIELAND_API UPieceLandSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void BeginDestroy() override;

	virtual void Deinitialize() override;

public:
	FCreatePieceLandEvent CreatePieceLandEvent;

	UPROPERTY(BlueprintAssignable)
	FPieceLandShowTipEvent PieceLandShowTipEvent;
	
	UPROPERTY(BlueprintAssignable)
	FPieceLandHideTipEvent PieceLandHideTipEvent;
	
protected:

	UPROPERTY()
	class UDataTable* LevelLandDataTable; //FLevelLandDataTable

	UPROPERTY()
	class UBasePieceLand* CurLand;

	int actionAutoId{1};
public:

	UFUNCTION(BlueprintCallable)
	void CreateLevelLand(FName levelName,bool forceCreate = false);

	UFUNCTION(BlueprintCallable)
	void AgainCurLevelLand();
	
	UFUNCTION(BlueprintCallable)
	UBasePieceLand* GetCurLand();

	UFUNCTION(BlueprintPure)
	bool CreateActionToPiece(FPieceActionHandle& handle,int pieceId,class UPieceBaseActionConfigData* actionData);
	
	UFUNCTION(BlueprintCallable)
	void DeleteActionByPiece(FPieceActionHandle handle);
};