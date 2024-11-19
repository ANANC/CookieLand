// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.generated.h"

class ACookieLandPieceActor;


UENUM(BlueprintType)
enum class ECookieLandPieceOrientation : uint8
{
	Up = 0,
	Down,
	Left,
	Right,
	Forward,
	Backward
};


USTRUCT(BlueprintType)
struct FCookieLandLocation 
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite ,EditAnywhere)
	int X = 0;
	UPROPERTY(BlueprintReadWrite ,EditAnywhere)
	int Y = 0;
	UPROPERTY(BlueprintReadWrite ,EditAnywhere)
	int Floor = 0;

public:
	FCookieLandLocation() { X = 0; Y = 0; Floor = 0; }
	FCookieLandLocation(int InX,int InY,int InFloor) { X = InX; Y = InY; Floor = InFloor; }
	FCookieLandLocation(FVector InVector) { X = InVector.X; Y = InVector.Y; Floor = InVector.Z; }

	bool operator==(const FCookieLandLocation& Other) const
	{
		return X == Other.X && Y == Other.Y && Floor == Other.Floor;
	}

	bool operator!=(const FCookieLandLocation& Other) const
	{
		return X != Other.X || Y != Other.Y || Floor != Other.Floor;
	}

	bool operator==(const FVector& Other) const
	{
		return X == Other.X && Y == Other.Y && Floor == Other.Z;
	}

	FVector ToVector() const
	{
		return FVector(X, Y, Floor);
	}
};



USTRUCT(BlueprintType)
struct FCookieLandPieceLocator
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation PieceOrientation;

public:
	FCookieLandPieceLocator(){}
	FCookieLandPieceLocator(FCookieLandLocation InPieceLocation, ECookieLandPieceOrientation InPieceOrientation) { PieceLocation = InPieceLocation; PieceOrientation = InPieceOrientation; }
};


USTRUCT(BlueprintType)
struct FCookieLandPieceBuildInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "坐标"))
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "方位"))
	ECookieLandPieceOrientation PieceOrientation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否自动构建实例"))
	bool bAutoCreateActorInstance = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地块类型"))
	TSubclassOf< ACookieLandPieceActor> PieceActorType = nullptr;

public:
	FCookieLandPieceBuildInfo(){}
	FCookieLandPieceBuildInfo(FCookieLandLocation InPieceLocation, ECookieLandPieceOrientation InPieceOrientation) { PieceLocation = InPieceLocation; PieceOrientation = InPieceOrientation; }
};

UCLASS(Abstract, Blueprintable, EditInlineNew)
class UCookieLandBaseConfigInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否使用默认配置"))
	bool bUseDefault = true;

public:
	virtual void CopyWhenUseDefault(const UCookieLandBaseConfigInfo* Default){}
};


UCLASS(Blueprintable,EditInlineNew)
class UCookieLandPieceActorTypeConfigInfo :public UCookieLandBaseConfigInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地块类型"))
	TSubclassOf< ACookieLandPieceActor> PieceActorType = nullptr;

public:
	UCookieLandPieceActorTypeConfigInfo() {}
	UCookieLandPieceActorTypeConfigInfo(TSubclassOf< ACookieLandPieceActor> InPieceActorType) { PieceActorType = InPieceActorType; }

	virtual void CopyWhenUseDefault(const UCookieLandBaseConfigInfo* Default) override
	{
		if (bUseDefault)
		{
			if (const UCookieLandPieceActorTypeConfigInfo* ValueDefault = Cast<UCookieLandPieceActorTypeConfigInfo>(Default))
			{
				PieceActorType = ValueDefault->PieceActorType;
			}
		}
	}
};

UCLASS(Blueprintable, EditInlineNew)
class  UCookieLandPieceBuildInfoConfigInfo :public UCookieLandBaseConfigInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地块配置"))
	FCookieLandPieceBuildInfo PieceBuildInfo;

public:
	UCookieLandPieceBuildInfoConfigInfo() {}
	UCookieLandPieceBuildInfoConfigInfo(FCookieLandPieceBuildInfo InPieceBuildInfo) { PieceBuildInfo = InPieceBuildInfo; }

	virtual void CopyWhenUseDefault(const UCookieLandBaseConfigInfo* Default) override
	{
		if (bUseDefault)
		{
			if (const UCookieLandPieceBuildInfoConfigInfo* ValueDefault = Cast<UCookieLandPieceBuildInfoConfigInfo>(Default))
			{
				PieceBuildInfo = ValueDefault->PieceBuildInfo;
			}
		}
	}
};


UCLASS(Blueprintable, EditInlineNew)
class UCookieLandVectorConfigInfo :public UCookieLandBaseConfigInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Value;

public:
	UCookieLandVectorConfigInfo(){}
	UCookieLandVectorConfigInfo(FVector InValue) { Value = InValue; }

	virtual void CopyWhenUseDefault(const UCookieLandBaseConfigInfo* Default) override
	{
		if (bUseDefault)
		{
			if (const UCookieLandVectorConfigInfo* ValueDefault = Cast<UCookieLandVectorConfigInfo>(Default))
			{
				Value = ValueDefault->Value;
			}
		}
	}
};

USTRUCT(BlueprintType)
struct FCookieLandMapBuildInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta = (DisplayName = "地块类型"))
	UCookieLandPieceActorTypeConfigInfo* PieceActorType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta = (DisplayName = "地块配置"))
	UCookieLandPieceBuildInfoConfigInfo* PieceBuildInfo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced,meta = (DisplayName = "坐标排版间隔"))
	UCookieLandVectorConfigInfo* LocationSortInterval;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta = (DisplayName = "方位排版间隔"))
	UCookieLandVectorConfigInfo* OrientationSortInterval;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta = (DisplayName = "初始坐标"))
	UCookieLandVectorConfigInfo* InitialLocation;

public:
	TArray< UCookieLandBaseConfigInfo*> GetPropertys()
	{
		return { PieceActorType,PieceBuildInfo,LocationSortInterval,OrientationSortInterval,InitialLocation };
	}
};


UCLASS(BlueprintType)
class UCookieLandMapPiecesDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地块数据"))
	TArray<FCookieLandPieceBuildInfo> PieceBuildInfos;
};

UCLASS(BlueprintType)
class UCookieLandMapBuildDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地形配置"))
	FCookieLandMapBuildInfo BuildInfo;
};

USTRUCT(BlueprintType)
struct FCookieLandMapBuildDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地形配置"))
	TObjectPtr<UCookieLandMapBuildDataAsset> BuildDataAsset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地块数据"))
	TObjectPtr<UCookieLandMapPiecesDataAsset> PiecesDataAsset = nullptr;
};
