// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Piece/PieceBaseAction.h"
#include "PieceAction_PopTip.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UPieceAction_PopTip : public UPieceBaseAction
{
	GENERATED_BODY()

protected:
	virtual void _Init() override;
	
	virtual void _UnInit() override;

public:
	static int DynamicTipId;
	
private:
	UPROPERTY()
	class UPieceActionConfigData_PopTip* ConfigData;

	int TriggerNumber{0};

	int tipId;

public:
	virtual void SetData(class UPieceBaseActionConfigData* data) override;

	virtual void Execute() override;

protected:
	void TryTriggerTip(int lastPieceId,int standPieceId);

	void TryDeleteTip(int lastPieceId,int standPieceId);

	UFUNCTION()
	void MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation);
};


UENUM(BlueprintType)
enum class EPieceActionPopTipTriggerType : uint8
{
	DoNothing,
	EnterPiece,
	ExitPiece,
};

UENUM(BlueprintType)
enum class EPieceActionPopTipDeleteType : uint8
{
	DoNothing,
	EnterPiece,
	ExitPiece,
};

UCLASS()
class UPieceActionConfigData_PopTip : public UPieceBaseActionConfigData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionPopTipTriggerType TriggerType{EPieceActionPopTipTriggerType::DoNothing};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsTriggerOnce{false};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionPopTipDeleteType DeleteType{EPieceActionPopTipDeleteType::DoNothing};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsDeleteAllTip{false};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsDeleteAllTypeTip{false};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FPiecePopTipData TipData;

	UPieceActionConfigData_PopTip(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
	{
		ActionClass = UPieceAction_PopTip::StaticClass();
	}
};