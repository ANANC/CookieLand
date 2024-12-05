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

	// 大于
	bool GetIsMaxByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
	{
		switch (Orientation)
		{
		case ECookieLandPieceOrientation::Up:
		case ECookieLandPieceOrientation::Down:
			return Floor > Other.Floor;
			break;
		case ECookieLandPieceOrientation::Left:
		case ECookieLandPieceOrientation::Right:
			return X > Other.X;
			break;
		case ECookieLandPieceOrientation::Forward:
		case ECookieLandPieceOrientation::Backward:
			return Y > Other.Y;
			break;
		};
		return false;
	}

	// 大于等于
	bool GetIsMaxEqualByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
	{
		switch (Orientation)
		{
		case ECookieLandPieceOrientation::Up:
		case ECookieLandPieceOrientation::Down:
			return Floor >= Other.Floor;
			break;
		case ECookieLandPieceOrientation::Left:
		case ECookieLandPieceOrientation::Right:
			return X >= Other.X;
			break;
		case ECookieLandPieceOrientation::Forward:
		case ECookieLandPieceOrientation::Backward:
			return Y >= Other.Y;
			break;
		};
		return false;
	}

	// 小于
	bool GetIsMinByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
	{
		switch (Orientation)
		{
		case ECookieLandPieceOrientation::Up:
		case ECookieLandPieceOrientation::Down:
			return Floor < Other.Floor;
			break;
		case ECookieLandPieceOrientation::Left:
		case ECookieLandPieceOrientation::Right:
			return X < Other.X;
			break;
		case ECookieLandPieceOrientation::Forward:
		case ECookieLandPieceOrientation::Backward:
			return Y < Other.Y;
			break;
		};
		return false;
	}

	// 小于等于
	bool GetIsMinEqualByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const
	{
		switch (Orientation)
		{
		case ECookieLandPieceOrientation::Up:
		case ECookieLandPieceOrientation::Down:
			return Floor <= Other.Floor;
			break;
		case ECookieLandPieceOrientation::Left:
		case ECookieLandPieceOrientation::Right:
			return X <= Other.X;
			break;
		case ECookieLandPieceOrientation::Forward:
		case ECookieLandPieceOrientation::Backward:
			return Y <= Other.Y;
			break;
		};
		return false;
	}

	void AddDistanceByThreeDirection(ECookieLandPieceOrientation Orientation, int32 Distance)
	{
		switch (Orientation)
		{
		case ECookieLandPieceOrientation::Up:
		case ECookieLandPieceOrientation::Down:
			Floor += Distance;
			break;
		case ECookieLandPieceOrientation::Left:
		case ECookieLandPieceOrientation::Right:
			X += Distance;
			break;
		case ECookieLandPieceOrientation::Forward:
		case ECookieLandPieceOrientation::Backward:
			Y += Distance;
			break;
		};
	}
	void AddDistanceBySixDirection(ECookieLandPieceOrientation Orientation, int32 Distance)
	{
		switch (Orientation)
		{
		case ECookieLandPieceOrientation::Up:
			Floor += Distance;
			break;
		case ECookieLandPieceOrientation::Down:
			Floor -= Distance;
			break;
		case ECookieLandPieceOrientation::Left:
			X += Distance;
			break;
		case ECookieLandPieceOrientation::Right:
			X -= Distance;
			break;
		case ECookieLandPieceOrientation::Forward:
			Y += Distance;
			break;
		case ECookieLandPieceOrientation::Backward:
			Y -= Distance;
			break;
		};
	}

	TArray<ECookieLandPieceOrientation> GetRelativeOrientations(FCookieLandLocation Location) const
	{
		TArray<ECookieLandPieceOrientation> RelativeOrientations;

		if (Location.X > X)
		{
			RelativeOrientations.Add(ECookieLandPieceOrientation::Right);
		}
		else if (Location.X < X)
		{
			RelativeOrientations.Add(ECookieLandPieceOrientation::Left);
		}

		if (Location.Y > Y)
		{
			RelativeOrientations.Add(ECookieLandPieceOrientation::Forward);
		}
		else if (Location.Y < Y)
		{
			RelativeOrientations.Add(ECookieLandPieceOrientation::Backward);
		}

		if (Location.Floor > Floor)
		{
			RelativeOrientations.Add(ECookieLandPieceOrientation::Up);
		}
		else if (Location.Floor < Floor)
		{
			RelativeOrientations.Add(ECookieLandPieceOrientation::Down);
		}

		return RelativeOrientations;
	}


	TMap<ECookieLandPieceOrientation,int> GetRelativeOrientationAndDistances(FCookieLandLocation Location) const
	{
		TMap<ECookieLandPieceOrientation, int> RelativeOrientationAndDistances;

		if (Location.X > X)
		{
			RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Right, Location.X - X);
		}
		else if (Location.X < X)
		{
			RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Left, X - Location.X);
		}

		if (Location.Y > Y)
		{
			RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Forward, Location.Y - Y);
		}
		else if (Location.Y < Y)
		{
			RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Backward, Y - Location.Y);
		}

		if (Location.Floor > Floor)
		{
			RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Up, Location.Floor - Floor);
		}
		else if (Location.Floor < Floor)
		{
			RelativeOrientationAndDistances.Add(ECookieLandPieceOrientation::Down, Floor - Location.Floor);
		}

		return RelativeOrientationAndDistances;
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

	bool operator==(const FCookieLandPieceLocator& Other) const
	{
		return PieceLocation == Other.PieceLocation && PieceOrientation == Other.PieceOrientation;
	}
};


// todo 基础移动行为
USTRUCT(BlueprintType)
struct FCookieLandPieceBaseAction
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "可移动方向"))
	TArray<ECookieLandPieceOrientation> EnableOrientations;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "阻碍方向"))
	TArray<ECookieLandPieceOrientation> ImpedeOrientations;
};

// todo 地块行为

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "基础行为"))
	FCookieLandPieceBaseAction BaseAction;
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

UENUM(BlueprintType)
enum class ECookieLandMapShowType : uint8
{
	Plane,
	ThreeDimensions,
};


USTRUCT(BlueprintType)
struct FCookieLandMapShowInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "显示类型"))
	ECookieLandMapShowType MapShowType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否限制显示层数"))
	bool bConfineFloor = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "限制显示的层数"), meta = (EditCondition = "bConfineFloor"))
	int ConfineFloorNumber = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否限制显示周围"))
	bool bConfineRound = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "限制显示的周围数量"), meta = (EditCondition = "bConfineRound"))
	int ConfineRoundNumber = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否立方体遮挡显示"))
	bool bCubeOcclusionDisplay = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否立方体只显示边缘"))
	bool bOnlyDisplayEgdeWhenCubeOcclusionDisplay = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否立方体边缘显示细节"))
	bool bDisplayDetailInEgdeWhenCubeOcclusionDisplay = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否立方体内部显示细节"))
	bool bDisplayDetailInSideWhenCubeOcclusionDisplay = false;
};

UCLASS(BlueprintType)
class UCookieLandMapShowDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "显示配置"))
	FCookieLandMapShowInfo MapShowInfo;
};


USTRUCT(BlueprintType)
struct FCookieLandMapBuildDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地形配置"))
	TObjectPtr<UCookieLandMapBuildDataAsset> BuildDataAsset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "显示配置"))
	TObjectPtr< UCookieLandMapShowDataAsset> ShowDataAsset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地块数据"))
	TObjectPtr<UCookieLandMapPiecesDataAsset> PiecesDataAsset = nullptr;
};


USTRUCT(BlueprintType)
struct FCookieLandPerceptualObjectPerceptionInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "感知范围"))
	int PerceptionRange = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "被感知范围"))
	int PassivePerceptionRange = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "能否在感知到其他感知者时，感知到周围地形"))
	bool bEnableSenseMapWhenPerception = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地形感知范围"), meta = (EditCondition = "bEnableSenseMapWhenPerception"))
	int SenseMapRange = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "能否在异域时，感知到其他感知者"))
	bool bEnablePerceptionWhereInForeignMap = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "能否在异域感知到其他感知者时，显示地块信息"))
	bool bEnableSenseDetailWhereInForeignMap = false;
};

USTRUCT(BlueprintType)
struct FCookieLandPerceptualObjectPerceptionDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "感知信息"))
	FCookieLandPerceptualObjectPerceptionInfo PerceptionInfo;

};

UENUM(BlueprintType)
enum class ECookieLandMapAngleViewType : uint8
{
	ForwardAndRight,
	ForwardAndLeft,
	BackwardAndLeft,
	BackwardAndRight,
};