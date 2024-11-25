// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.h"
#include "CookieLandPiece.generated.h"

class UCookieLandPiece;
class UCookieLandMapShowDirector;

UCLASS(Blueprintable,BlueprintType)
class COOKIELAND_API ACookieLandPieceActor : public AActor
{
	GENERATED_BODY()

protected:

	bool bInit = false;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation PieceOrientation;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr< UCookieLandPiece> CookieLandPiece = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite)
	UCookieLandMapShowDirector* MapShowDirector;

public:
	virtual void BeginDestroy() override;

	virtual void Init(UCookieLandPiece* InCookieLandPiece);
	virtual void UnInit();

	bool GetIsInit();

public:
	void SetMapShowDirector(UCookieLandMapShowDirector* InMapShowDirector) { MapShowDirector = InMapShowDirector; }

	virtual void UpdateDisplay(){}
};


UCLASS()
class COOKIELAND_API UCookieLandPiece : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite)
	FCookieLandPieceBuildInfo BuildInfo;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACookieLandPieceActor> PieceActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite)
	ECookieLandPieceOrientation PieceOrientation;

protected:
	bool bInit = false;

public:
	virtual void Init(FCookieLandPieceBuildInfo InBuildInfo);
	virtual void UnInit();

	// 设置实例
	virtual void SetPieceActor(ACookieLandPieceActor* Instance);

	// 设置实例类型
	void SetPieceActorType(TSubclassOf< ACookieLandPieceActor> PieceActorType);

	// 获取实例
	ACookieLandPieceActor* GetPieceAction();

	// 获取坐标
	FCookieLandLocation GetPieceLocation();

	// 获取方位
	ECookieLandPieceOrientation GetPieceOrientation();

	// 获取构建信息
	FCookieLandPieceBuildInfo GetBuildInfo();

};
