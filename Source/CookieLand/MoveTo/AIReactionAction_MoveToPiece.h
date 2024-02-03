// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIReactionAction_BaseMoveTo.h"
#include "UObject/Object.h"
#include "AIReactionAction_MoveToPiece.generated.h"

class UAIReactionMoveToPieceActionData;
/**
 * 
 */
UCLASS()
class COOKIELAND_API UAIReactionAction_MoveToPiece : public UAIReactionAction_BaseMoveTo
{
	GENERATED_BODY()

public:
	virtual void SetActionData(UAIReactionBaseMoveToActionData* data) override;
	
protected:
	virtual void CalculateMoveToPath() override;

protected:
	UPROPERTY()
	TObjectPtr<UAIReactionMoveToPieceActionData> MoveToPieceActionData;
};

UCLASS(BlueprintType)
class UAIReactionMoveToPieceActionData :public UAIReactionBaseMoveToActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int TargetPieceId;

public:

	UAIReactionMoveToPieceActionData(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
	{
		ActionClass = UAIReactionAction_MoveToPiece::StaticClass();
	}
};