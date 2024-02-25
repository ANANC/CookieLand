// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "PieceTypes.generated.h"


UENUM(BlueprintType)
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
	int X{0};
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Y{0};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Floor{0};

	FPieceLocation(){}
	FPieceLocation(bool isValue):IsValid(isValue){}
	FPieceLocation(int x,int y,int floor):X(x),Y(y),Floor(floor){}
	FPieceLocation(FVector vector):X(vector.X),Y(vector.Y),Floor(vector.Z){}
	FPieceLocation(FPieceLocation& other):X(other.X),Y(other.Y),Floor(other.Floor){}
	FPieceLocation(FPieceLocation& other,int floor):X(other.X),Y(other.Y),Floor(floor){}
	
	bool operator==(const FPieceLocation& Other) const
	{
		return IsValid && Other.IsValid && X == Other.X && Y == Other.Y && Floor == Other.Floor;
	}
	bool operator!=(const FPieceLocation& Other) const
	{
		return !IsValid || !Other.IsValid || X != Other.X || Y != Other.Y || Floor != Other.Floor;
	}
	void operator+=(const FPieceLocation& Other)
	{
		if(IsValid && Other.IsValid)
		{
			X += Other.X;
			Y += Other.Y;
			Floor += Other.Floor;
		}
	}

	void operator-=(const FPieceLocation& Other)
	{
		if(IsValid && Other.IsValid)
		{
			X -= Other.X;
			Y -= Other.Y;
			Floor -= Other.Floor;
		}
	}
	
	void Copy(FPieceLocation Other)
	{
		IsValid = Other.IsValid;
		X = Other.X;
		Y = Other.Y;
		Floor = Other.Floor;
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

USTRUCT(BlueprintType)
struct FPieceActionHandle
{
	GENERATED_USTRUCT_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsValid{false};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Id{-1};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int PieceId{-1};

public:
	bool operator==(const FPieceActionHandle& Other) const
	{
		return IsValid && Other.IsValid && Id == Other.Id;
	}

	FPieceActionHandle(){}
	FPieceActionHandle(int id,int pieceId):Id(id),PieceId(pieceId){IsValid=true;}

	int GetPieceId(){return PieceId;}
	bool GetIsValid(){return IsValid;}
	void Clear(){IsValid = false;Id = -1;}
};


UCLASS(abstract,EditInlineNew,Blueprintable)
class COOKIELAND_API UPieceBaseActionConfigData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsMustAttachToValidPiece{true};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsAutoExecute{true};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsAutoFinish{true};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class UPieceBaseAction> ActionClass;
};

UCLASS(EditInlineNew,Blueprintable)
class COOKIELAND_API UPieceBaseConfigData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Id{0};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class ABasePieceActor> ActorClass = nullptr;
	
	UPROPERTY(EditAnywhere,Instanced)
	UPieceBaseInfo* BaseInfo;
	
	UPROPERTY(EditAnywhere,Instanced)
	TArray<TObjectPtr<UPieceBaseActionConfigData>> Actions;
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

	UPROPERTY(EditAnywhere,Instanced)
	TObjectPtr<UPieceBaseConfigData> DefaultPieceConfig;
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsNextInOrder{true};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName NextTargetLevel;
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
	TArray<class UBasePiece*> Pieces;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MaxX{0};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MinX{0};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MaxY{0};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MinY{0};
};

  
UCLASS()
class UPieceLandBoundInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<int,UPieceLandFloorBoundInfo*> Floor2BoundInfos;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<int> ValidFloors;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MaxFloor{0};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MinFloor{0};
	
public:
	void AddPiece(class UBasePiece* piece);

	void RemovePiece(class UBasePiece* piece);

	void UpdateBound();

	bool HasValidFloors();

	bool HasValidPieceInFloor(int floor);

	UPieceLandFloorBoundInfo* GetFloorBoundInfo(int floor);
};

USTRUCT(BlueprintType)
struct FPiecePopTipData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Id{-1};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsFixType{false};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float RemainTime{3.f};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FText Tip;

	FPiecePopTipData(){};
	FPiecePopTipData(FPiecePopTipData& source){
		Id = source.Id;
		IsFixType = source.IsFixType;
		RemainTime = source.RemainTime;
		Tip = source.Tip;
	}
	
};


UENUM(BlueprintType)
enum class EPieceActionState : uint8
{
	Empty,
	DropOut,
	Rotation,
};

USTRUCT(BlueprintType)
struct FAddPieceActionStateRequest
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPieceActionState State;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsReqMainState{true};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool EnableReplaceMainState{true};

	FAddPieceActionStateRequest(){}
	FAddPieceActionStateRequest(EPieceActionState state):State(state){}
	FAddPieceActionStateRequest(EPieceActionState state,bool isReqMainState,bool enableReplaceMainState):
	State(state),IsReqMainState(isReqMainState),EnableReplaceMainState(enableReplaceMainState){}
};

UCLASS()
class COOKIELAND_API UPieceBaseActionRunTimeInfo : public UObject
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EPieceActionRotationAngleType : uint8
{
	//90°
	Clockwise,
	//-90°
	Anticlockwise,
	//水平-180°
	Horizontal,
	//垂直-上下交换
	Vertical,
};


USTRUCT(BlueprintType)
struct FPieceCardConfigData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName CardName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (MultiLine = "true"))
	FText DescribeName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (MultiLine = "true"))
	FText Describe;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int ValidDistance{3};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsOnlyCross{true};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsHorizontal{true};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsVertical{true};
	
	UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite)
	TObjectPtr<UPieceBaseActionConfigData> ActionConfigData;
	
};


UCLASS(BlueprintType)
class COOKIELAND_API UPieceCardCollectionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FPieceCardConfigData> Cards;
};



UENUM(BlueprintType)
enum class EPieceObserveFormType : uint8
{
	Full,
	OnlySimpleLine,
};

USTRUCT(BlueprintType)
struct FPieceObserveStateData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsVisible{false};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPieceObserveFormType ObserveFormType{EPieceObserveFormType::Full};

	FPieceObserveStateData(){}
	FPieceObserveStateData(bool isVisible):IsVisible(isVisible){}
};