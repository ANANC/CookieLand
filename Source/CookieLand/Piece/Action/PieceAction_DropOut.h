// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Piece/PieceBaseAction.h"
#include "PieceAction_DropOut.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UPieceAction_DropOut : public UPieceBaseAction
{
	GENERATED_BODY()

	
protected:
	virtual void _Init() override;
	
	virtual void _UnInit() override;

private:
	UPROPERTY()
	class UPieceActionConfigData_DropOut* ConfigData;

	FTimerHandle DelayTimerHandle;
	
public:
	virtual void SetData(class UPieceBaseActionConfigData* data) override;

	virtual void Execute() override;
	
protected:
	void TryTriggerTip(int standPieceId);

	UFUNCTION()
	void MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation);
	
	UFUNCTION()
	void TriggerDropOut();
};

UENUM(BlueprintType)
enum class EPieceActionDropOutTriggerType : uint8
{
	EnterPiece,
};

UCLASS()
class UPieceActionConfigData_DropOut : public UPieceBaseActionConfigData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionDropOutTriggerType TriggerType;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DelayTime{2.f};

	UPieceActionConfigData_DropOut(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
	{
		ActionClass = UPieceAction_DropOut::StaticClass();
	}
};
