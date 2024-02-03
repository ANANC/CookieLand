// Fill out your copyright notice in the Description page of Project Settings.


#include "AIReactionAction_BaseMoveTo.h"

#include "CookieLand/Character/BaseCharacter.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

const FName UAIReactionAction_BaseMoveTo::CounterName_UpdatePathInterval(TEXT( "UpdatePathInterval" ));
const FName UAIReactionAction_BaseMoveTo::CounterName_MoveInterval(TEXT( "MoveInterval" ));
const FName UAIReactionAction_BaseMoveTo::CounterName_RunTime(TEXT( "RunTime" ));
const FName UAIReactionAction_BaseMoveTo::CounterName_RotationTime(TEXT( "RotationTime" ));

void UAIReactionAction_BaseMoveTo::SetAI(class ABaseCharacter* aiCharacter)
{
   Character = aiCharacter;
   CharacterInstanceName = UKismetSystemLibrary::GetDisplayName(aiCharacter);
}

void UAIReactionAction_BaseMoveTo::SetActionData(UAIReactionBaseMoveToActionData* data)
{
    Data = Cast<UAIReactionBaseMoveToActionData>(data);

    ShowDebugLine = Data->ShowDebugLine;

    IsStartMove = false;
    IsForceFinish = false;

    CreateCounter(CounterName_MoveInterval,0);
    
    CreateCounter(CounterName_UpdatePathInterval,Data->UpdatePathInterval);
    GetCounter(CounterName_UpdatePathInterval)->SetEnableTrigger(Data->IsUpdatePathInterval);
    
    CreateCounter(CounterName_RotationTime,Data->RotationSpeedTime);
    GetCounter(CounterName_RotationTime)->SetEnableTrigger(false);
    
    CreateCounter(CounterName_RunTime,FMath::RandRange(Data->MinMoveTime, Data->MaxMoveTime));
    GetCounter(CounterName_RunTime)->SetEnableTrigger(Data->MinMoveTime == -1 && Data->MaxMoveTime == -1?false:true);
    
    curRotationAlpha = 0;
    targetYaw = -1;
    curYaw = -1;

    curTargetPoint = FVector::ZeroVector;
    
    PathLocations.Reset();
}

void UAIReactionAction_BaseMoveTo::Init()
{
   SetExecuteState(EAIConditionReactionActionExecuteState::Continue);
}

void UAIReactionAction_BaseMoveTo::UnInit()
{
    CleanMovementEnvironment();
}

void UAIReactionAction_BaseMoveTo::SetExecuteState(EAIConditionReactionActionExecuteState state)
{
   ExecuteState = state;
}

EAIConditionReactionActionExecuteState UAIReactionAction_BaseMoveTo::GetExecuteState()
{
   return ExecuteState;
}

void UAIReactionAction_BaseMoveTo::StartMove()
{
    if (Character.Get()) 
    {
       lastCharacterLocation = Character->GetActorLocation();
    }
}

void UAIReactionAction_BaseMoveTo::TickMoveTo(float delta)
{
   DeltaTimeSeconds = delta;
   
    if (!IsStartMove)
    {
       IsStartMove = true;
       StartMove();
       
       ResetCalculatePath();

       if(CheckIsArrive(true))
       {
          SetForceFinish("Tick. Arrive Finish.",FColor::Blue);
       }
    }

    do
    {
       if (IsForceFinish)
       {
          break;
       }

       if (!Character.Get())
       {
          SetForceFinish("Tick. !Character.Get() || !AIController.Get()",FColor::White);
          break;
       }
       
       GetCounter(CounterName_UpdatePathInterval)->UpdateCutTime(DeltaTimeSeconds);
       GetCounter(CounterName_MoveInterval)->UpdateCutTime(DeltaTimeSeconds);
       GetCounter(CounterName_RunTime)->UpdateCutTime(DeltaTimeSeconds);
       
       if(GetCounter(CounterName_RunTime)->GetIsTrigger())
       {
          if(ShowDebugLine)
          {
             UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove. curTime>=TotalMoveTime curTime:%f TotalMoveTime:%f"),*CharacterInstanceName,GetCounter(CounterName_RunTime)->GetCurCount(),GetCounter(CounterName_RunTime)->GetTotalCount());
          }
          SetForceFinish("Tick. curTime>=TotalMoveTime",FColor::Red);
          break;
       }

       if(GetCounter(CounterName_UpdatePathInterval)->GetIsTrigger())
       {
          GetCounter(CounterName_UpdatePathInterval)->ResetCurCount();
      
          if(ShowDebugLine)
          {
             UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  ResetCalculatePath.  in time. cur:%f total:%f"),*CharacterInstanceName,GetCounter(CounterName_UpdatePathInterval)->GetCurCount(),GetCounter(CounterName_UpdatePathInterval)->GetTotalCount());
          }
          
          ResetCalculatePath();
       }

       if(GetCounter(CounterName_MoveInterval)->GetIsTrigger())
       {
          GetCounter(CounterName_MoveInterval)->ResetCurCount();
          ControlMoveTo(DeltaTimeSeconds);
       }
       
       if (IsForceFinish)
       {
          break;
       }

       Move();
       
       if(!GetEnableMove())
       {
          GetCounter(CounterName_RunTime)->UpdateCutTime(-DeltaTimeSeconds);
          GetCounter(CounterName_UpdatePathInterval)->ResetCurCount();
       }
       
       if(CheckIsArrive())
       {
          if(PathLocations.Num() == 0)
          {
             SetForceFinish("Tick. Arrive",FColor::Blue);
             break;
          }
          else          {
             if(ShowDebugLine)
             {
                UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove. arrive curTargetPoint."),*CharacterInstanceName);
             }
             GetCounter(CounterName_MoveInterval)->SetTrigger();
          }
       }
       
       if(ShowDebugLine)
       {
          FVector myLocation = Character->GetTransform().GetLocation();
          DrawDebugPoint(GetWorld(), myLocation, 3, FColor::White, false, 0.3f);
          DrawDebugPoint(GetWorld(), curTargetPoint, 20, FColor::Black, false, 0.5f);
       }
    } while (false);

    if (IsForceFinish)
    {
       EndMove();
    }
}

void UAIReactionAction_BaseMoveTo::RotationFinish()
{
    SetEnableMove(true,"RotationFinish");
}

void UAIReactionAction_BaseMoveTo::TriggerUpdatePath()
{
   if(ShowDebugLine)
   {
      UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  ResetCalculatePath. TriggerUpdatePath."),*CharacterInstanceName);
   }
   ResetCalculatePath();
   //GetCounter(CounterName_MoveInterval)->ResetCurCount();
}

void UAIReactionAction_BaseMoveTo::ResetCalculatePath()
{
    if(ShowDebugLine)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove. ResetCalculatePath"),*CharacterInstanceName);
    }
    
    GetCounter(CounterName_UpdatePathInterval)->ResetCurCount();
    
    CalculateMoveToPath();

    CalculateNavigablePath(CalculatePathLocations);
}

void UAIReactionAction_BaseMoveTo::CalculateMoveToPath(){}

void UAIReactionAction_BaseMoveTo::CalculateNavigablePath(TArray<FVector> paths)
{
    PathLocations.Empty();

    FVector myLocation = Character->GetTransform().GetLocation();

    float distance = 0;
    for (int index = 0; index < paths.Num(); ++index)
    {
       FVector pathLocation = paths[index];
       
   
       PathLocations.Add(pathLocation);
       if(PathLocations.Num()==1)
       {
          distance += FVector::Distance(pathLocation,myLocation);
       }else
       {
          distance += FVector::Distance(pathLocation,PathLocations[PathLocations.Num()-2]);
       }
    }

    DebugCalculatePath(Data->DebugShowTime);

    if (PathLocations.Num() == 0)
    {
       SetForceFinish("CalculateNavigablePath. PathLocations.Num() == 0",FColor::Green);
       return;
    }

    if(ShowDebugLine)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  Update PathLocations. %d"),*CharacterInstanceName,PathLocations.Num());    
    }

    GetCounter(CounterName_MoveInterval)->SetTrigger();
}

void UAIReactionAction_BaseMoveTo::RefreshMoveInfo(float distance)
{
    FVector myLocation = Character->GetTransform().GetLocation();

    movementInput = (curTargetPoint - myLocation).GetSafeNormal();

    if(Data->IsFixedSpeed && Data->FixedSpeed>0)
    {
       CurSpeed = Data->FixedSpeed;
    }
    else    {
       CurSpeed = distance / GetCounter(CounterName_RunTime)->GetTotalCount();
       CurSpeed = FMath::Max(Data->MaxMoveSpeed, CurSpeed);
       CurSpeed = FMath::Min(Data->MinMoveSpeed, CurSpeed);
    }

    AcceleratedSpeedCompensationTime = CurSpeed<=500?DeltaTimeSeconds * 5:0;//加速度补偿时间        if(ShowDebugLine)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  RefreshMoveInfo. speed:%f"),*CharacterInstanceName,CurSpeed);  
    }

   Character->GetCharacterMovementComponent()->MaxWalkSpeed = CurSpeed;
}

void UAIReactionAction_BaseMoveTo::ControlMoveTo(float deltaTimeSeconds)
{
    bool updatePath = false;
   
    if(curTargetPoint!=FVector::ZeroVector)
    {
       FVector myLocation = Character->GetActorLocation();
       float distanceSquared = FVector::DistSquared2D(myLocation,curTargetPoint);
       float checkDistance = Data->Acceptance *2;
       if(distanceSquared>checkDistance*checkDistance)
       {
          if(Data->ShowDebugLine)
          {
             float distance = FVector::Dist2D(myLocation,curTargetPoint);
             UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove. ResetCalculatePath. too far. cur:%s target:%s distance:%f checkDistance:%f"),*CharacterInstanceName,*myLocation.ToString(),*curTargetPoint.ToString(),distance,checkDistance);
          }

          updatePath = true;
       }
    }

    if(updatePath)
    {
       ResetCalculatePath();
    }
    
    if (PathLocations.Num() == 0)
    {
       SetForceFinish("ControlMoveTo. PathLocations.Num() == 0",FColor::Green);
       return;
    }
    
    RequestPathNextLocation();
}

void UAIReactionAction_BaseMoveTo::RequestPathNextLocation()
{
    if(PathLocations.Num() == 0)
    {
       if(ShowDebugLine)
       {
          UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove. RequestPathNextLocation Fail. PathLocations.Num() == 0"),*CharacterInstanceName);
       }
       
       GetCounter(CounterName_MoveInterval)->SetTrigger();
       return;
    }

    FVector point = PathLocations[0];
    PathLocations.RemoveAt(0);

    FVector myLocation = Character->GetTransform().GetLocation();
    float distance =  FVector::Distance(myLocation,point);
    if(PathLocations.Num()>0 && distance<Data->MoveNextMinDistance)
    {
       if(ShowDebugLine)
       {
          UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  over proximity. to next. point:%s distance:%f"),*CharacterInstanceName,*point.ToString(),distance);
       }
       RequestPathNextLocation();
       return;
    }

    curTargetPoint = point;
    RefreshMoveInfo(distance);
    
    UpdateTargetYaw();
    
    GetCounter(CounterName_MoveInterval)->SetTotalCount(distance/CurSpeed + AcceleratedSpeedCompensationTime);
    AcceleratedSpeedCompensationTime = 0;

    GetCounter(CounterName_MoveInterval)->ResetCurCount();
    
    if(ShowDebugLine)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  NextPoint. %s count:%d movePrecisionTime:%f distance:%f"),*CharacterInstanceName,*curTargetPoint.ToString(),PathLocations.Num(),GetCounter(CounterName_MoveInterval)->GetTotalCount(),distance);
    }
}


void UAIReactionAction_BaseMoveTo::UpdateTargetYaw()
{
    FVector myLocation = Character->GetActorLocation();
    FRotator myRotator = Character->K2_GetActorRotation();
    
    curYaw = UCommonFunctionLibrary::ChangeToControllerYaw(myRotator.Yaw);

    FVector lookatPoint = curTargetPoint;
    lookatPoint.Z = myLocation.Z;
    FRotator lookAtRotator = UKismetMathLibrary::FindLookAtRotation(myLocation, lookatPoint);
    float calculateYaw = UCommonFunctionLibrary::GetSmoothRotationYaw(curYaw,lookAtRotator.Yaw );
    targetYaw = UCommonFunctionLibrary::ChangeToControllerYaw(calculateYaw);

    offsetYaw = UCommonFunctionLibrary::GetDifferenceValueByControllerYaw(curYaw,targetYaw);
    
    float rotationTime;
    float absDiffYaw = abs(offsetYaw);
    if(absDiffYaw>=1)
    {
       rotationTime = Data->RotationSpeedTime;
       if(absDiffYaw<=45){rotationTime*=0.2f;}
       else if(absDiffYaw<=90){rotationTime*=0.5f;}
       else if(absDiffYaw<=135){rotationTime*=0.8f;}
    
       GetCounter(CounterName_RotationTime)->ResetCurCount();
       GetCounter(CounterName_RotationTime)->SetTotalCount(rotationTime);
       GetCounter(CounterName_RotationTime)->SetEnableTrigger(true);
    }
    else
    {
       rotationTime = 0;
       GetCounter(CounterName_RotationTime)->SetEnableTrigger(false);
    }
    
    if(ShowDebugLine)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  UpdateTargetYaw. rotation curYaw:%f lookatYaw:%f calculateYaw:%f targetYaw:%f diffYaw:%f rotationTime:%f"),*CharacterInstanceName,curYaw,lookAtRotator.Yaw,calculateYaw,targetYaw,offsetYaw,rotationTime);
    }
    
    if(absDiffYaw < Data->MoveRotationMaxAngle) 
    {
       SetEnableMove(true,"offsetYaw<=Data->MoveRotationMaxAngle");
    }
    else
    {
       SetEnableMove(false,"offsetYaw>Data->MoveRotationMaxAngle");
    }
}

void UAIReactionAction_BaseMoveTo::Move()
{
   FRotator rotator = Character->GetActorRotation();
   if(GetCounter(CounterName_RotationTime)->GetEnableTrigger())
   {
      GetCounter(CounterName_RotationTime)->UpdateCutTime(DeltaTimeSeconds);
          
      curRotationAlpha = GetCounter(CounterName_RotationTime)->GetCurCount()/GetCounter(CounterName_RotationTime)->GetTotalCount();
      curRotationAlpha = curRotationAlpha>1?1:curRotationAlpha;

      curRotationAlpha = abs(offsetYaw)<5? 1:curRotationAlpha;//小范围直接旋转到目标位置
      float newYaw = UCommonFunctionLibrary::ChangeToControllerYaw(curYaw + offsetYaw * curRotationAlpha);
      rotator.Yaw = newYaw;
       
      Character->SetActorRotation(rotator);
       
      GetCounter(CounterName_MoveInterval)->SetTotalCount(GetCounter(CounterName_MoveInterval)->GetTotalCount()+(curRotationAlpha==1?0:DeltaTimeSeconds));
       
      if(curRotationAlpha == 1)
      {
         GetCounter(CounterName_RotationTime)->ResetCurCount();
         GetCounter(CounterName_RotationTime)->SetEnableTrigger(false);
             
         RotationFinish();
      }
   }
   else
   {
      rotator.Yaw = targetYaw;
   }
       
   if(GetEnableMove())
   {
      float forwardAlpha = 1.f;
      if( GetCounter(CounterName_RotationTime)->GetEnableTrigger())
      {
         forwardAlpha = 0.4f + (0.8f-0.4f)*curRotationAlpha;
      }
          
      FVector myForward = rotator.Quaternion().GetForwardVector();
          
      //Character->GetLocomotionComponent()->MoveForward(forwardAlpha);
      Character->GetMovementComponent()->Velocity = myForward * (forwardAlpha * CurSpeed);

      /*       if(ShowDebugLine)
       *       {
       *          UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.     move forward. speed:%s"),*CharacterInstanceName,*Character->GetVelocity().ToString());
       *       }
       */
   }
}

void UAIReactionAction_BaseMoveTo::DebugCalculatePath(float showTime)
{
    if (!ShowDebugLine || !Character.Get())
    {
       return;
    }

    for (int index = 0; index < CalculatePathLocations.Num(); ++index)
    {
       FVector startPoint = CalculatePathLocations[index];
       FVector endPoint = startPoint;

       if (index < CalculatePathLocations.Num() - 1)
       {
          endPoint = CalculatePathLocations[index + 1];
       }

       DrawDebugPoint(Character->GetWorld(), startPoint, 10, FColor::Blue, false, showTime);
       DrawDebugLine(Character->GetWorld(), startPoint, endPoint, FColor::Blue, false, showTime);
    }

    for (int index = 0; index < PathLocations.Num(); ++index)
    {
       FVector startPoint = PathLocations[index];
       FVector endPoint = startPoint;

       if (index < PathLocations.Num() - 1)
       {
          endPoint = PathLocations[index + 1];
       }

       DrawDebugPoint(Character->GetWorld(), startPoint, 10, FColor::Green, false, showTime);
       DrawDebugLine(Character->GetWorld(), startPoint, endPoint, FColor::Green, false, showTime);
    }
    
}

bool UAIReactionAction_BaseMoveTo::CheckIsArrive(FVector targetLocation)
{
    FVector curLocation = Character->GetActorLocation();
    float distSquared = FVector::DistSquared2D(curLocation,targetLocation);
    float acceptanceSquared = Data->Acceptance*Data->Acceptance;
    if(distSquared<=acceptanceSquared)
    {
       return true;
    }
    return false;
}

bool UAIReactionAction_BaseMoveTo::CheckIsArrive(bool useFinishLocation)
{
    if(!Character.Get())
    {
       return true;
    }
    
    FVector lastLocation = lastCharacterLocation;
    FVector curLocation = Character->GetActorLocation();
    float lastToCurDistace = FVector::Dist2D(lastLocation,curLocation);

    FVector targetLocation = useFinishLocation?(PathLocations.Num()>0?PathLocations.Last():curTargetPoint):curTargetPoint;
    lastCharacterLocation = curLocation;

    float interval = 1;//检测间隔
    FVector direction = (curLocation - lastLocation).GetSafeNormal();
    float acceptanceSquared = Data->Acceptance*Data->Acceptance;
    if(!useFinishLocation){acceptanceSquared*=0.5f;}
    
    for(int index = 0;index<lastToCurDistace;index += interval)
    {
       FVector point = lastLocation + index*direction;

       float distSquared = FVector::DistSquared2D(point,targetLocation);
       if(distSquared<=acceptanceSquared)
       {
          return true;
       }
    }

    return false;
}


void UAIReactionAction_BaseMoveTo::EndMove()
{
    SetExecuteState(EAIConditionReactionActionExecuteState::Succeeded);
}

void UAIReactionAction_BaseMoveTo::CleanMovementEnvironment()
{
    SetForceFinish("CleanMovementEnvironment",FColor::White);
    
    EnableMove = false;
    EnableMoveLock = false;
    
    FinishToCleanEnvironment();

    if(Character.Get()&& Data.Get()&&Data->ShowDebugLine)
    {
       DrawDebugPoint(GetWorld(), Character->GetActorLocation(), 12, FColor::Red, false, 0.5f);
    }
}

void UAIReactionAction_BaseMoveTo::FinishToCleanEnvironment(){}

void UAIReactionAction_BaseMoveTo::SetForceFinish(FString souceDebug,FColor debugColor)
{
    IsForceFinish = true;

    if(Character.Get()&& Data.Get() && Data->ShowDebugLine)
    {
       DrawDebugPoint(GetWorld(), Character->GetActorLocation(), 15, debugColor, false, 0.5f);
    }
    
    if(Data?ShowDebugLine:false)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove. SetForceFinish. %s"),*CharacterInstanceName,*souceDebug);
    }
}

bool UAIReactionAction_BaseMoveTo::GetIsForceFinish()
{
    return IsForceFinish;
}

int UAIReactionAction_BaseMoveTo::GetPathLocationsNumber()
{
    return  PathLocations.Num();
}

float UAIReactionAction_BaseMoveTo::GetCurSpeed()
{
    return CurSpeed;
}

void UAIReactionAction_BaseMoveTo::SetEnableMoveLock(bool isLock,FString log)
{
    EnableMoveLock = isLock;
    if(Data->ShowDebugLine)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  SetEnableMoveLock. %d %s"),*CharacterInstanceName,EnableMoveLock?1:0,*log);
    }
}

void UAIReactionAction_BaseMoveTo::SetEnableMove(bool enable,FString log)
{
    if(EnableMoveLock)
    {
       return;
    }
    
    EnableMove = enable;
    if(Data->ShowDebugLine)
    {
       UE_LOG(LogTemp, Log, TEXT("[xqa] [%s] AIMove.  SetEnableMove. %d %s"),*CharacterInstanceName,EnableMove?1:0,*log);
    }
}

bool UAIReactionAction_BaseMoveTo::GetEnableMove()
{
    return EnableMove;
}

UAIReactionAction_BaseMoveTo_BaseCounter* UAIReactionAction_BaseMoveTo::CreateCounter(FName name,float total)
{
    UAIReactionAction_BaseMoveTo_BaseCounter* counter = nullptr;
    UAIReactionAction_BaseMoveTo_BaseCounter** counterPtr = Name2Counters.Find(name);
    if(counterPtr)
    {
       counter = *counterPtr;
       counter->ResetCurCount();
    }
    else    {
       counter = NewObject<UAIReactionAction_BaseMoveTo_BaseCounter>();
       Name2Counters.Add(name,counter);
    }

    counter->SetName(name);
    counter->SetTotalCount(total);
    counter->SetEnableTrigger(true);
    return counter;
}

UAIReactionAction_BaseMoveTo_BaseCounter* UAIReactionAction_BaseMoveTo::GetCounter(FName name)
{
    UAIReactionAction_BaseMoveTo_BaseCounter** counterPtr = Name2Counters.Find(name);
    if(counterPtr)
    {
       return *counterPtr;
    }
    
    UE_LOG(LogTemp,Error,TEXT("[xqa] [%s] AIMove. GetCounter Failed. %s"),*CharacterInstanceName,*name.ToString());
    
    counterPtr = Name2Counters.Find("Default");
    if(!counterPtr)
    {
       return CreateCounter("Default",1);
    }
    return *counterPtr;
}