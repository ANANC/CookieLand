// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Piece/PieceBaseAction.h"
#include "PieceAction_Rotation.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UPieceAction_Rotation : public UPieceBaseAction
{
	GENERATED_BODY()
	
protected:
	virtual void _Init() override;
	
	virtual void _UnInit() override;

private:
	UPROPERTY()
	class UPieceActionConfigData_Rotation* ConfigData;
	
	UPROPERTY()
	class UPieceActionRunTimeInfo_Rotation* RuntimeInfo;
	
	bool IsTriggering{false};

	FTimerHandle RotationIntervalTimerHandle;

	int RotationNumber{0};
	float LastRotationTime{0};
	
public:
	virtual void SetData(class UPieceBaseActionConfigData* data) override;

	virtual void Execute() override;

protected:
	UFUNCTION()
	void MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation);
	
	void TriggerRotation();

	void RotationPiece();

	float GetRotationAngle();
};

UENUM(BlueprintType)
enum class EPieceActionRotationTriggerType : uint8
{
	AutoTrigger,
	PlayerMove,
	EnterPiece,
	ExitPiece,
};

UENUM(BlueprintType)
enum class EPieceActionRotationCountType : uint8
{
	Single,
	Number,
	ContinueTime,
	Continue,
};

UCLASS()
class UPieceActionConfigData_Rotation : public UPieceBaseActionConfigData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionRotationTriggerType TriggerType{EPieceActionRotationTriggerType::PlayerMove};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionRotationAngleType RotationAngle{EPieceActionRotationAngleType::Clockwise};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceActionRotationCountType RotationCount{EPieceActionRotationCountType::Single};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "RotationCount == EPieceActionRotationCountType::Number", EditConditionHides))
	int RotationNumber{1};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "RotationCount == EPieceActionRotationCountType::ContinueTime", EditConditionHides))
	float ContinueTime{1};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "RotationCount != EPieceActionRotationCountType::Single", EditConditionHides))
	float IntervalTime{0.5f};
	
	UPieceActionConfigData_Rotation(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
	{
		ActionClass = UPieceAction_Rotation::StaticClass();
	}
};

UCLASS()
class COOKIELAND_API UPieceActionRunTimeInfo_Rotation : public UPieceBaseActionRunTimeInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float RotationAngle;
};
