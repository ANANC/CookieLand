// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandBaseAnimTask.generated.h"
 
class UCookieLandBasePieceAction;
class UCookieLandPiece;

UCLASS()
class COOKIELAND_API UCookieLandBaseAnimTask : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Init(int InId, UCookieLandBasePieceAction* InAction);

	virtual void Active();
	virtual void Finish();

protected:
	int Id = -1;

	UPROPERTY()
	TObjectPtr<UCookieLandBasePieceAction> Action = nullptr;

	UPROPERTY()
	TObjectPtr< UCookieLandPiece> Piece = nullptr;

public:
	int GetId();

	UCookieLandBasePieceAction* GetAction();

	UCookieLandPiece* GetPiece();
};
