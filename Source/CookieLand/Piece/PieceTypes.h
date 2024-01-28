// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "PieceTypes.generated.h"


UENUM()
enum class EPieceDirection : uint8
{
	Left,
	Right,
	Forward,
	Backward,
	Up,
	Down,
};


USTRUCT(BlueprintType)
struct COOKIELAND_API FPieceLocation
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsValid{true};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int X;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Y;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Floor;
	
	bool operator==(const FPieceLocation& Other) const
	{
		return IsValid && Other.IsValid && X == Other.X && Y == Other.Y && Floor == Other.Floor;
	}
};

USTRUCT(BlueprintType)
struct COOKIELAND_API FPieceDistance
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsUnLimit{true};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int LimitDistance{1};
};

UCLASS(EditInlineNew,Blueprintable)
class COOKIELAND_API UPieceBaseInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FPieceLocation Location;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FPieceDistance Distance;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<EPieceDirection> EnableDirections{EPieceDirection::Left,EPieceDirection::Right,EPieceDirection::Forward,EPieceDirection::Backward};

public:
	void Copy(UPieceBaseInfo* sourceInfo)
	{
		Location = sourceInfo->Location;
		Distance = sourceInfo->Distance;
		EnableDirections.Reset();
		for(int index = 0;index<sourceInfo->EnableDirections.Num();++index)
		{
			EnableDirections.Add(sourceInfo->EnableDirections[index]);
		}
	}
};

UCLASS(abstract,EditInlineNew,Blueprintable)
class COOKIELAND_API UPieceBaseStateConfigData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Name;
};


UCLASS(EditInlineNew,Blueprintable)
class COOKIELAND_API UPieceBaseConfigData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Id{0};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class ABasePieceActor> ActorClass;
	
	UPROPERTY(EditAnywhere,Instanced)
	UPieceBaseInfo* BaseInfo;
	
	UPROPERTY(EditAnywhere,Instanced)
	TArray<TObjectPtr<UPieceBaseStateConfigData>> StateConfigData;

};

UCLASS(BlueprintType)
class COOKIELAND_API ULandDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector CreateInitialLocation{0,0,0};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector ActorInterval{50,50,100};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class ABasePieceActor> DefaultActorClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int InitialPieceId{-1};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int FinishPieceId{-1};
	
	UPROPERTY(EditAnywhere,Instanced)
	TArray<TObjectPtr<UPieceBaseConfigData>> Pieces;
};

USTRUCT(BlueprintType)
struct COOKIELAND_API FLevelLandDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class ULandDataAsset* LandDataAsset;
};


UCLASS()
class UPieceInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int Id;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UPieceBaseInfo* Info;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool IsOccupy{false};
};

UCLASS()
class UPieceLocationInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FPieceLocation Location;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsOccupy{false};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int PieceId{-1};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool EnableOccupy{true};
};

UCLASS()
class UPieceLandFloorBoundInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Floor;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector Bound;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector Center;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<int> PieceIds;
};


UCLASS()
class UPieceLandBoundInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<int,UPieceLandFloorBoundInfo*> Floor2BoundInfos;
};