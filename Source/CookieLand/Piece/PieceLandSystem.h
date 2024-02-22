// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PieceTypes.h"
#include "UObject/Object.h"
#include "PieceLandSystem.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatePieceLandEvent, FName, int);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPieceLandShowTipEvent, FPiecePopTipData, tipData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPieceLandHideTargetTipEvent, int, tipId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPieceLandHideAllTipEvent, bool, OnlyTypeTip, bool, isFixType);

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
	FPieceLandHideTargetTipEvent PieceLandHideTargetTipEvent;
	
	UPROPERTY(BlueprintAssignable)
	FPieceLandHideAllTipEvent PieceLandHideAllTipEvent;
	
protected:

	UPROPERTY()
	class UDataTable* LevelLandDataTable; //FLevelLandDataTable

	UPROPERTY()
	class UPieceCardCollectionDataAsset* PieceCardCollectionDataAsset; //UPieceCardCollectionDataAsset
	
	UPROPERTY()
	class UBasePieceLand* CurLand;

public:

	UFUNCTION(BlueprintCallable)
	void CreateLevelLand(FName levelName,bool forceCreate = false);

	UFUNCTION(BlueprintCallable)
	void AgainCurLevelLand();
	
	UFUNCTION(BlueprintCallable)
	UBasePieceLand* GetCurLand();
	
	UFUNCTION(BlueprintCallable)
	void ToNextLand();

	UFUNCTION(BlueprintPure)
	TArray<FName> GetAllPieceCardNames();

	UFUNCTION(BlueprintPure)
	bool GetPieceCardConfigData(FName cardName,FPieceCardConfigData& pieceCardConfigData);

	
};