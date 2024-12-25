// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLandBasePieceAction.h"
#include "CookieLandMapTypes.h"
#include "PieceAction_Move.generated.h"

UENUM(BlueprintType)
enum class EPieceActionMoveTriggerType : uint8
{
	Auto,
	Stand
};

UENUM(BlueprintType)
enum class EPieceActionMoveType : uint8
{
	NextPiece,
	Fixed
};

UCLASS(Blueprintable, EditInlineNew)
class COOKIELAND_API UPieceActionData_Move : public UCookieLandBasePieceActionData
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "移动类型"))
	EPieceActionMoveTriggerType Trigger = EPieceActionMoveTriggerType::Auto;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "移动类型"))
	EPieceActionMoveType MoveType = EPieceActionMoveType::NextPiece;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "固定距离"), meta = (EditCondition = "MoveType == EPieceActionMoveType::Fixed"))
	int FixedDistance = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "移动方向"))
	ECookieLandPieceOrientation MoveOrientation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否无视障碍物"))
	bool bIngoreObstacle = false;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};

UCLASS()
class COOKIELAND_API UPieceAction_Move : public UCookieLandBasePieceAction
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPieceActionData_Move* Data;

protected:
	virtual void SetData(UCookieLandBasePieceActionData* InData) override;

public:

	virtual void Active() override;

protected:

	FCookieLandPieceLocator StartLocator;
	FCookieLandPieceLocator EndLocator;

protected:

	FCookieLandPieceLocator GetMoveTarget();
};
