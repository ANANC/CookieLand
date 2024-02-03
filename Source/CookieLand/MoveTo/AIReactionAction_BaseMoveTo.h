// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AIReactionAction_BaseMoveTo.generated.h"

UENUM(BlueprintType)
enum class EAIConditionReactionActionExecuteState : uint8
{
    Succeeded,
    Failed,
    Continue
};

UCLASS()
class UAIReactionAction_BaseMoveTo_BaseCounter:public UObject{
    GENERATED_BODY()
public:
    FName Name;
    bool EnableTrigger{true};
    float TotalCount{0};
    float CutCount{0};

public:    
    void SetName(FName name){Name = name;}
    void SetTotalCount(float total){TotalCount = total;}
    float GetTotalCount(){return TotalCount;}
    bool UpdateCutTime(float delta){CutCount+=delta;CutCount = FMath::Max(CutCount,0.f);return GetIsTrigger();};    void ResetCurCount(){CutCount = 0;}
    float GetCurCount(){return CutCount;}
    void SetEnableTrigger(bool enable){EnableTrigger = enable;}
    bool GetEnableTrigger(){return EnableTrigger;}
    void SetTrigger(){CutCount = TotalCount;}
    bool GetIsTrigger(){return EnableTrigger&&CutCount>=TotalCount;}
};

class UAIReactionBaseMoveToActionData;

UCLASS(Abstract,abstract)
class UAIReactionAction_BaseMoveTo : public UObject{
    GENERATED_BODY()

public:
    virtual void SetAI(class ABaseCharacter* aiCharacter);

    virtual void SetActionData(UAIReactionBaseMoveToActionData* data);

    virtual void Init();
    
    virtual void UnInit();

    virtual void SetExecuteState(EAIConditionReactionActionExecuteState state);

    EAIConditionReactionActionExecuteState GetExecuteState();
    
    UFUNCTION()
    virtual void TickMoveTo(float delta);
    
protected:
    TWeakObjectPtr<class ABaseCharacter> Character;
    FString CharacterInstanceName;
    
    TArray<FVector> CalculatePathLocations;

    EAIConditionReactionActionExecuteState ExecuteState;
    
    bool ShowDebugLine;

    float DeltaTimeSeconds;

protected:        
    static const FName CounterName_UpdatePathInterval;
    static const FName CounterName_MoveInterval;
    static const FName CounterName_RunTime;
    static const FName CounterName_RotationTime;
    
private:
    bool IsForceFinish;
    bool IsStartMove;

    bool EnableMove{true};
    bool EnableMoveLock{false};

    FVector curTargetPoint;
    FVector movementInput;

    UPROPERTY()
    TMap<FName,UAIReactionAction_BaseMoveTo_BaseCounter*> Name2Counters;

    FVector lastCharacterLocation;
    float curRotationAlpha{0};
    float targetYaw{-1};
    float curYaw{-1};
    float offsetYaw{-1};
    
    UPROPERTY()
    TObjectPtr<class UAIReactionBaseMoveToActionData> Data;

    TArray<FVector> PathLocations;

    float CurSpeed;
    float AcceleratedSpeedCompensationTime;
    
protected:

    virtual void StartMove();

    virtual void EndMove();

    virtual void CalculateMoveToPath();

    void CalculateNavigablePath(TArray<FVector> paths);

    void SetForceFinish(FString souceDebug,FColor debugColor);

    bool GetIsForceFinish();
    
    virtual void ControlMoveTo(float deltaTimeSeconds);

    int GetPathLocationsNumber();

    virtual void FinishToCleanEnvironment();

    float GetCurSpeed();

    void SetEnableMoveLock(bool isLock,FString log);
    
    void SetEnableMove(bool enable,FString log);

    bool GetEnableMove();
    
    virtual void RotationFinish();

    void TriggerUpdatePath();

    UAIReactionAction_BaseMoveTo_BaseCounter* CreateCounter(FName name,float total);
    
    UAIReactionAction_BaseMoveTo_BaseCounter* GetCounter(FName name);
    
    bool CheckIsArrive(FVector targetLocation);
    
private:
    void Move();
    
    void RefreshMoveInfo(float distance);

    void CleanMovementEnvironment();

    bool CheckIsArrive(bool useFinishLocation = false);
    
    void DebugCalculatePath(float showTime = 5);

    void UpdateTargetYaw();

    void ResetCalculatePath();
    
    void RequestPathNextLocation();
};

    
UCLASS(BlueprintType)
class UAIReactionBaseMoveToActionData :public UObject{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    bool IsFixedSpeed{true};

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float FixedSpeed{500};
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float MinMoveTime{-1};

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float MaxMoveTime{-1};

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float MinMoveSpeed{500};

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float MaxMoveSpeed{500};

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    bool IsUpdatePathInterval{false};
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float UpdatePathInterval{0.5f};
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float Acceptance{50};
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float MoveRotationMaxAngle{5};
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float RotationSpeedTime{0.2f};

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float MoveNextMinDistance{130};
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    bool ShowDebugLine{false};

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float DebugShowTime{5.f};
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    TSubclassOf<UAIReactionAction_BaseMoveTo> ActionClass;
public:

    UAIReactionBaseMoveToActionData(const FObjectInitializer& ObjectInitializer)
    {
        ActionClass = UAIReactionAction_BaseMoveTo::StaticClass();
    }
};