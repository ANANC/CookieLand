// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPieceBaseOComponent.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPieceBaseOComponent : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COOKIELAND_API IPieceBaseOComponent
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void SetPiece(class UBasePiece* piece) = 0;
	virtual void Init() = 0;
	virtual void UnInit() = 0;
};
