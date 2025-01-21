// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.h"
#include "Runtime/GameplayTags/Classes/GameplayTagContainer.h"
#include "CookieLandActionTypes.generated.h"

#define GET_GAMEPLAY_TAG(TagName) FGameplayTag::RequestGameplayTag(FName(TagName))


// -- Task --
#pragma region Task

#pragma region UAnimTask_Move

// 移动
USTRUCT(BlueprintType)
struct COOKIELAND_API FAnimTaskMoveData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "移动曲线"))
	UCurveFloat* MoveCurvel = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "持续时间"))
	float Duration = 1;
};

#pragma endregion

#pragma endregion


// -- Action --
#pragma region Action

class UCookieLandBasePieceAction;
class ACookieLandBaseCueActor;

// 基础Cue
USTRUCT(BlueprintType)
struct COOKIELAND_API FCueActorData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "CueActor标记符"))
	FGameplayTag CueActorTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "CueActor类型"))
	TSubclassOf< ACookieLandBaseCueActor> CueActorType = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,Instanced, meta = (DisplayName = "CueActor数据"))
	UCookieLandBaseCueActorData* CueActorData = nullptr;
};

// 基础ActionData
UCLASS(Blueprintable)
class COOKIELAND_API UCookieLandBasePieceActionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "事件类型"))
	TSubclassOf< UCookieLandBasePieceAction> PieceActionClassType = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否允许重复添加"))
	bool bEnableRepetition = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否立即执行"))
	bool bActiveImmediately = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "CueActor列表"))
	TArray<FCueActorData> CueActorDatas;
};

#pragma region UPieceAction_Move

UENUM(BlueprintType)
enum class EPieceActionMoveTriggerType : uint8
{
	Auto,
	Stand
};

UENUM(BlueprintType)
enum class EPieceActionMoveType : uint8
{
	NextPiece,
	Fixed
};

// 移动
UCLASS(Blueprintable, EditInlineNew)
class COOKIELAND_API UPieceActionData_Move : public UCookieLandBasePieceActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "触发类型"))
	EPieceActionMoveTriggerType Trigger = EPieceActionMoveTriggerType::Auto;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "移动类型"))
	EPieceActionMoveType MoveType = EPieceActionMoveType::NextPiece;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "固定距离"), meta = (EditCondition = "MoveType == EPieceActionMoveType::Fixed"))
	int FixedDistance = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "移动方向"))
	ECookieLandPieceOrientation MoveOrientation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "是否无视障碍物"))
	bool bIngoreObstacle = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Task:移动"))
	FAnimTaskMoveData AnimTaskMoveData;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};

#pragma endregion

#pragma region UPieceAction_MoveBackAndForth

// 来回移动
UCLASS(Blueprintable, EditInlineNew)
class COOKIELAND_API UPieceActionData_MoveBackAndForth : public UCookieLandBasePieceActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "来回触发类型"))
	EPieceActionMoveTriggerType AgainTrigger = EPieceActionMoveTriggerType::Auto;

};
#pragma endregion

#pragma endregion

// -- Cue -- 
#pragma region Cue


// 基础CueData
UCLASS(Blueprintable, EditInlineNew)
class COOKIELAND_API UCookieLandBaseCueActorData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "绑定在PieceActor上"))
	bool bAttachPieceActor = true;
};

#pragma region UCookieLandCueData_Move
UCLASS(Blueprintable, EditInlineNew)
class COOKIELAND_API UCookieLandCueData_Move : public UCookieLandBaseCueActorData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "移动方向"))
	ECookieLandPieceOrientation MoveOrientation;
};
#pragma endregion


#pragma endregion
