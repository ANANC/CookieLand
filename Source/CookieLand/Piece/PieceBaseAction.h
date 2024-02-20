// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PieceTypes.h"
#include "PieceBaseAction.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UPieceBaseAction : public UObject
{
	GENERATED_BODY()

public:
	void SetHandle(FPieceActionHandle handle);
	
	virtual void SetPiece(class UBasePiece* piece);
	
	virtual void SetData(class UPieceBaseActionConfigData* data);
	
	void Init();

	virtual void Execute();

	bool IsFinish();
	
	void UnInit();

	FPieceActionHandle GetHandle();
	
private:
	bool isFinish{false};

	UPROPERTY()
	FPieceActionHandle Handle;

	UPROPERTY()
	class UPieceBaseActionConfigData* BaseConfigData;
protected:
	int PieceId;
	
	UPROPERTY()
	class UBasePiece* Piece;
	
protected:
	virtual void _Init();
	
	virtual void _UnInit();

	void SetIsFinish(bool value);
};
