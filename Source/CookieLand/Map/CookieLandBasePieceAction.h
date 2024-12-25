// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandBasePieceAction.generated.h"


class UCookieLandBasePieceAction;
class UCookieLandPiece;
class ACookieLandMapBuildActor;
class UCookieLandMapBuilder;
class UCookieLandBaseAnimTask;

UCLASS(Blueprintable, EditInlineNew)
class COOKIELAND_API UCookieLandBasePieceActionData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "事件类型"))
	TSubclassOf< UCookieLandBasePieceAction> PieceActionClassType = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否允许重复添加"))
	bool bEnableRepetition = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否立即执行"))
	bool bActiveImmediately = true;
};

UCLASS(Blueprintable,BlueprintType)
class COOKIELAND_API UCookieLandBasePieceAction : public UObject
{
	GENERATED_BODY()
	
protected:

	int Id = -1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UCookieLandBasePieceActionData* BaseData = nullptr;

	UPROPERTY()
	TObjectPtr< UCookieLandPiece> Piece;

	UPROPERTY()
	TArray< TObjectPtr<UCookieLandBaseAnimTask>> Tasks;

	int AutoTaskId = 0;
public:
	virtual void Init(int InId, UCookieLandPiece* InPiece,UCookieLandBasePieceActionData* InData);
	virtual void UnInit();

	virtual void Active();
	virtual void Finish();

public:
	int GetId() const;

	UCookieLandPiece* GetPiece();

	ACookieLandMapBuildActor* GetMapBuildActor();

	UCookieLandMapBuilder* GetMapBuilder();

	const UCookieLandBasePieceActionData* GetData() const;

protected:
	virtual void SetData(UCookieLandBasePieceActionData* InData);

public:

	void AddTask();
};
