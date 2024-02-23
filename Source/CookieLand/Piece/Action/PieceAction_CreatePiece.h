// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Piece/PieceBaseAction.h"
#include "PieceAction_CreatePiece.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UPieceAction_CreatePiece : public UPieceBaseAction
{
	GENERATED_BODY()
		
protected:
	virtual void _Init() override;
	
	virtual void _UnInit() override;

private:
	UPROPERTY()
	class UPieceActionConfigData_CreatePiece* ConfigData;
	
public:
	virtual void SetData(class UPieceBaseActionConfigData* data) override;

	virtual void Execute() override;

protected:
	
	UFUNCTION()
	void MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation);
	
	void TriggerCreatePiece();
	
	FPieceLocation GetCreateLocation();

	void CreatePiece(FPieceLocation location);
};

UENUM(BlueprintType)
enum class EPieceActionCreatePieceTriggerType : uint8
{
	Immediately,
	EnterPiece,
};


UENUM(BlueprintType)
enum class EPieceActionCreatePieceLocationType : uint8
{
	BaseOnCharacter,
	BaseOnCurLocation,
	TargetLocation,
};

UCLASS()
class UPieceActionConfigData_CreatePiece : public UPieceBaseActionConfigData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsForceReplace{false};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionCreatePieceTriggerType TriggerType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionCreatePieceLocationType LocationType;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "LocationType != EPieceActionRotationCountType::TargetLocation", EditConditionHides))
	FPieceLocation RelativeCreateLocation;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "LocationType == EPieceActionRotationCountType::TargetLocation", EditConditionHides))
	FPieceLocation TargetCreateLocation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsUseDefaultPieceConfig{true};
	
	UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite,meta = (EditCondition = "IsUseDefaultPieceConfig == false", EditConditionHides))
	TObjectPtr<UPieceBaseConfigData> PieceConfig;
	
	UPieceActionConfigData_CreatePiece(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
	{
		IsMustAttachToValidPiece = false;
		ActionClass = UPieceAction_CreatePiece::StaticClass();
	}
};
