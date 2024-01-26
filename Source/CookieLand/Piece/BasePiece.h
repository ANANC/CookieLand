// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PieceTypes.h"
#include "BasePiece.generated.h"



UCLASS()
class COOKIELAND_API UBasePiece : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UBasePiece(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY()
	class UBasePieceLand* OwnLand;
	
	UPROPERTY()
	class UPieceBaseConfigData* ConfigData;
	
	UPROPERTY(VisibleAnywhere)
	class UPieceInfo* CurInfo;
	
	UPROPERTY(VisibleAnywhere)
	class ABasePieceActor* PieceActor;

public:
	void SetId(int Id);

	int GetId();

	void SetOwnLand(class UBasePieceLand* land);

	class UBasePieceLand* GetOwnLand();
	
	void SetConfigData(class UPieceBaseConfigData* configData);

	void Init();

	void UnInit();

public:
	UFUNCTION(BlueprintPure)
	const UPieceInfo* GetCurInfo();

	UFUNCTION(BlueprintPure)
	const UPieceBaseConfigData* GetConfigData();
	
	UFUNCTION(BlueprintPure)
	bool GetEnableMove(EPieceDirection direction);
	
protected:


};
