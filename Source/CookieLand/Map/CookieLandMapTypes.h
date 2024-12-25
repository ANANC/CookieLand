// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.generated.h"

class ACookieLandPieceActor;
class UCookieLandBasePieceActionData;

// 基础方向
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


// 基础坐标
USTRUCT(BlueprintType)
struct COOKIELAND_API FCookieLandLocation
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
	bool GetIsMaxByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const;

	// 大于等于
	bool GetIsMaxEqualByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const;

	// 小于
	bool GetIsMinByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const;

	// 小于等于
	bool GetIsMinEqualByOrientation(ECookieLandPieceOrientation Orientation, const FCookieLandLocation& Other) const;

	// 基于三方向添加距离
	void AddDistanceByThreeDirection(ECookieLandPieceOrientation Orientation, int32 Distance);

	// 基于六方向添加距离
	void AddDistanceBySixDirection(ECookieLandPieceOrientation Orientation, int32 Distance);

	// 获取相对方向
	TArray<ECookieLandPieceOrientation> GetRelativeOrientations(FCookieLandLocation Location) const;

	// 获取相对方向的距离
	TMap<ECookieLandPieceOrientation, int> GetRelativeOrientationAndDistances(FCookieLandLocation Location) const;

	// 获取相对方向的绝对距离
	TMap<ECookieLandPieceOrientation, int> GetAbsRelativeDistances(FCookieLandLocation Location) const;
};


// 基础位置
USTRUCT(BlueprintType)
struct COOKIELAND_API FCookieLandPieceLocator
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


// 基础行为
USTRUCT(BlueprintType)
struct FCookieLandPieceBaseAction
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "可移动方向"))
	TArray<ECookieLandPieceOrientation> EnableOrientations = { ECookieLandPieceOrientation::Left,ECookieLandPieceOrientation::Right,ECookieLandPieceOrientation::Forward,ECookieLandPieceOrientation::Backward };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "阻碍方向"))
	TArray<ECookieLandPieceOrientation> ImpedeOrientations;
};

// 地块构建信息
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere,Instanced, meta = (DisplayName = "行为数据"))
	TArray< UCookieLandBasePieceActionData*>  PieceActionDatas;

public:
	FCookieLandPieceBuildInfo(){}
	FCookieLandPieceBuildInfo(FCookieLandLocation InPieceLocation, ECookieLandPieceOrientation InPieceOrientation) { PieceLocation = InPieceLocation; PieceOrientation = InPieceOrientation; }
};


// 基础配置信息类
UCLASS(Abstract, Blueprintable, EditInlineNew)
class COOKIELAND_API UCookieLandBaseConfigInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否使用默认配置"))
	bool bUseDefault = true;

public:
	virtual void CopyWhenUseDefault(const UCookieLandBaseConfigInfo* Default){}
};


// 配置信息-地块类型
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

// 配置信息-地块配置
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

// 配置信息-向量
UCLASS(Blueprintable, EditInlineNew)
class COOKIELAND_API UCookieLandVectorConfigInfo :public UCookieLandBaseConfigInfo
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


// 地形构建信息
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


// 地块构建集合DA
UCLASS(BlueprintType)
class UCookieLandMapPiecesDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地块数据"))
	TArray<FCookieLandPieceBuildInfo> PieceBuildInfos;
};

// 地形构建DA
UCLASS(BlueprintType)
class UCookieLandMapBuildDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地形配置"))
	FCookieLandMapBuildInfo BuildInfo;
};

// 地形显示类型
UENUM(BlueprintType)
enum class ECookieLandMapShowType : uint8
{
	Plane,
	ThreeDimensions,
};

// 地形显示类型
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

// 地形显示DA
UCLASS(BlueprintType)
class UCookieLandMapShowDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "显示配置"))
	FCookieLandMapShowInfo MapShowInfo;
};

// 地图构建配置表
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

// 感知者感知地形配置信息
USTRUCT(BlueprintType)
struct FCookieLandPerceptualObjectSenseMapInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "能否感知"))
	bool bEnablePerceive = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地形感知范围"), meta = (EditCondition = "bEnablePerceive"))
	int SenseMapRoundRange = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "能否在感知到其他层级"), meta = (EditCondition = "bEnablePerceive"))
	bool bEnableSenseOtherFloor = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "地形感知层数范围"), meta = (EditCondition = "bEnablePerceive && bEnableSenseOtherFloor"))
	int SenseMapFloorRange = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "能否显示地块信息"))
	bool bEnableSenseDetail = false;

public:
	FCookieLandPerceptualObjectSenseMapInfo() {}

	FCookieLandPerceptualObjectSenseMapInfo(bool bInEnablePerceive) :bEnablePerceive(bInEnablePerceive) {}

	FCookieLandPerceptualObjectSenseMapInfo(bool bInEnablePerceive, int InSenseMapRoundRange, bool bInEnableSenseOtherFloor, int InSenseMapFloorRange, bool bInEnableSenseDetail) :
		bEnablePerceive(bInEnablePerceive), SenseMapRoundRange(InSenseMapRoundRange), bEnableSenseOtherFloor(bInEnableSenseOtherFloor), SenseMapFloorRange(InSenseMapFloorRange), bEnableSenseDetail(bInEnableSenseDetail)
	{}
};

// 感知者感知配置信息
USTRUCT(BlueprintType)
struct FCookieLandPerceptualObjectPerceptionInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "【感知者】感知其他感知者范围"))
	int PerceptionRange = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "【感知者】被其他感知者感知范围"))
	int PassivePerceptionRange = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "【感知者】能否感知其他层级的感知者"))
	bool bEnablePerceiveOtherFloor = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "【感知者】能否感知其他方向的感知者"))
	bool bEnablePerceiveOtherOrientation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "【地形】感知周围地形配置"))
	FCookieLandPerceptualObjectSenseMapInfo SenseMapRound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "【感知者】能否在异域时，感知到其他感知者"))
	bool bEnablePerceiveWhereInForeignMap = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "【地形】感知异域地形配置"))
	FCookieLandPerceptualObjectSenseMapInfo SenseForeignMap;

public:
	FCookieLandPerceptualObjectPerceptionInfo()
	{
		SenseForeignMap.bEnablePerceive = false;
	}
};

// 感知者配置表
USTRUCT(BlueprintType)
struct FCookieLandPerceptualObjectPerceptionDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "感知信息"))
	FCookieLandPerceptualObjectPerceptionInfo PerceptionInfo;

};

// 地图可视方向类型
UENUM(BlueprintType)
enum class ECookieLandMapAngleViewType : uint8
{
	ForwardAndRight,
	ForwardAndLeft,
	BackwardAndLeft,
	BackwardAndRight,
};


// 方向连接信息
USTRUCT(BlueprintType)
struct FCookieLandOrientationLinkInfo
{
	GENERATED_USTRUCT_BODY()

public:
	// 范围[右，上，前]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation Max_PieceLocation;

	// 范围[左，下，后]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation Min_PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation Orientation;

protected:
	bool bValid = false;

	// 距离 没有/只有一个=0
	int Distance = 0;

public:
	bool GetIsValid() const { return bValid; }
	int GetDistance() const { return Distance; }
	void SetData(ECookieLandPieceOrientation InOrientation, FCookieLandLocation InMax, FCookieLandLocation InMin);
	void SetMax(FCookieLandLocation InMax);
	void SetMin(FCookieLandLocation InMin);
	void AddLocation(FCookieLandLocation Location);
	void ClearData();

	// 是否在范围内[Min,Max]
	bool GetIsInSide(FCookieLandLocation Location);
protected:
	void UpdateDistance();

public:
	bool operator==(const FCookieLandOrientationLinkInfo& Other) const
	{
		if (!GetIsValid() && !Other.GetIsValid())
		{
			return true;
		}
		return GetIsValid() && Other.GetIsValid() && Max_PieceLocation == Other.Max_PieceLocation && Min_PieceLocation == Other.Min_PieceLocation && Orientation == Other.Orientation;
	}
};