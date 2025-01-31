// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandPiece.h"
#include "CookieLandMapBuildLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "CookieLandMapShowDirector.h"
#include "CookieLand/PerceptualObject/CookieLandPerceptualObjectComponent.h"
#include "CookieLandBasePieceAction.h"
#include "CookieLandBaseCueActor.h"

#pragma region ACookieLandPieceActor

ACookieLandPieceActor::ACookieLandPieceActor(const FObjectInitializer& Initializer) :
	Super(Initializer)
{
	EnterBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EnterBox"));
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

	FScriptDelegate OnComponentBeginOverlapDelegate;
	OnComponentBeginOverlapDelegate.BindUFunction(this, FName("ReceiveEnterBoxOnComponentBeginOverlapCallback"));
	EnterBox->OnComponentBeginOverlap.Add(OnComponentBeginOverlapDelegate);
}

void ACookieLandPieceActor::BeginDestroy()
{
	UnInit();
	Super::BeginDestroy();
}

void ACookieLandPieceActor::Init(UCookieLandPiece* InCookieLandPiece)
{
	if (!InCookieLandPiece)
	{
		return;
	}

	bInit = true;

	CookieLandPiece = InCookieLandPiece;

	PieceLocation = InCookieLandPiece->GetPieceLocation();
	PieceOrientation = InCookieLandPiece->GetPieceOrientation();

}

void ACookieLandPieceActor::UnInit()
{
	bInit = false;
	CookieLandPiece = nullptr;

	if (MapShowDirector)
	{
		MapShowDirector->TriggerPieceActorRenderUpdateEvent.RemoveAll(this);
	}
}

bool ACookieLandPieceActor::GetIsInit()
{
	return bInit;
}

void ACookieLandPieceActor::SetMapShowDirector(UCookieLandMapShowDirector* InMapShowDirector)
{ 
	MapShowDirector = InMapShowDirector;

	if (MapShowDirector)
	{
		FScriptDelegate TriggerPieceActorRenderUpdateEventDelegate;
		TriggerPieceActorRenderUpdateEventDelegate.BindUFunction(this, FName("ReceiveEnterBoxOnComponentBeginOverlapCallback"));
		MapShowDirector->TriggerPieceActorRenderUpdateEvent.Add(TriggerPieceActorRenderUpdateEventDelegate);
	}
}

FCookieLandPieceLocator ACookieLandPieceActor::GetLocator() const
{
	return FCookieLandPieceLocator(PieceLocation, PieceOrientation);
}

void ACookieLandPieceActor::ReceiveTriggerPieceActorRenderUpdateEventCallback(FCookieLandPieceLocator Locator)
{
	if (Locator == GetLocator())
	{
		UpdateDisplay();
	}
}

void ACookieLandPieceActor::ReceiveEnterBoxOnComponentBeginOverlapCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		UCookieLandPerceptualObjectComponent* PerceptualObjectComponent = OtherActor->GetComponentByClass<UCookieLandPerceptualObjectComponent>();
		if (PerceptualObjectComponent)
		{
			EnterPiece(PerceptualObjectComponent);
		}
	}
}

void ACookieLandPieceActor::EnterPiece(UCookieLandPerceptualObjectComponent* PerceptualObjectComponent)
{
	PerceptualObjectComponent->SetLocator(GetLocator());
}

bool ACookieLandPieceActor::GetOnEdge_Implementation(FVector Location)
{
	return false;
}
#pragma endregion

#pragma region UCookieLandPiece

void UCookieLandPiece::Init(FCookieLandPieceBuildInfo InBuildInfo)
{
	bInit = true;
	BuildInfo = InBuildInfo;
	PieceLocation = BuildInfo.PieceLocation;
	PieceOrientation = BuildInfo.PieceOrientation;
	BaseAttribute = BuildInfo.BaseAttribute;
}

void UCookieLandPiece::Active()
{
	for (int Index = 0; Index < BuildInfo.PieceActionDatas.Num(); ++Index)
	{
		UCookieLandBasePieceActionData* PieceActionData = BuildInfo.PieceActionDatas[Index];
		if (PieceActionData)
		{
			AddPieceAction(PieceActionData);
		}
	}
}

void UCookieLandPiece::UnInit()
{
	bInit = false;

	ClearPieceActions();

	if (PieceActor.Get())
	{
		PieceActor->Destroy();
	}
	PieceActor = nullptr;

	MapBuildActor = nullptr;
}

void UCookieLandPiece::SetMapBuildActor(ACookieLandMapBuildActor* InMapBuildActor)
{
	MapBuildActor = InMapBuildActor;
}

ACookieLandMapBuildActor* UCookieLandPiece::GetMapBuildActor()
{
	if (MapBuildActor) 
	{
		return MapBuildActor.Get();
	}
	return nullptr;
}

void UCookieLandPiece::SetPieceActor(ACookieLandPieceActor* Instance)
{
	PieceActor = Instance;
}

ACookieLandPieceActor* UCookieLandPiece::GetPieceActor()
{
	return PieceActor;
}

void UCookieLandPiece::SetPieceActorType(TSubclassOf< ACookieLandPieceActor> PieceActorType)
{
	BuildInfo.PieceActorType = PieceActorType;
}

void UCookieLandPiece::SetBaseAttribute(FCookieLandPieceBaseAttribute InBaseAttribute)
{
	BaseAttribute = InBaseAttribute;
}

ACookieLandPieceActor* UCookieLandPiece::GetPieceAction()
{
	return PieceActor;
}

FCookieLandLocation UCookieLandPiece::GetPieceLocation()
{
	return PieceLocation;
}

ECookieLandPieceOrientation UCookieLandPiece::GetPieceOrientation()
{
	return PieceOrientation;
}

FCookieLandPieceLocator UCookieLandPiece::GetPieceLocator()
{
	return FCookieLandPieceLocator(PieceLocation, PieceOrientation);
}

FCookieLandPieceBaseAttribute UCookieLandPiece::GetBaseAttribute()
{
	return BaseAttribute;
}

FCookieLandPieceBuildInfo UCookieLandPiece::GetBuildInfo()
{
	return BuildInfo;
}

void UCookieLandPiece::AddPieceAction(UCookieLandBasePieceActionData* PieceActionData)
{
	if (!PieceActionData || PieceActionData->PieceActionClassType == nullptr)
	{
		return;
	}

	// 不允许重复
	if (!PieceActionData->bEnableRepetition)
	{
		if (const UCookieLandBasePieceAction* RecordPieceAction = GetConstPieceActionByType(PieceActionData->PieceActionClassType))
		{
			return;
		}
	}

	UCookieLandBasePieceAction* PieceAction = NewObject< UCookieLandBasePieceAction>(this, PieceActionData->PieceActionClassType);
	if (!PieceAction)
	{
		return;
	}

	PieceActions.Add(PieceAction);

	AutoPieceActionId += 1;

	PieceAction->Init(AutoPieceActionId,this, PieceActionData);

	if (PieceActionData->bActiveImmediately)
	{
		PieceAction->Active();
	}
}

void UCookieLandPiece::RemovePieceActionByType(TSubclassOf<UCookieLandBasePieceAction> PieceActionClassType)
{
	UCookieLandBasePieceAction* PieceAction = GetPieceActionByType(PieceActionClassType);
	if (!PieceAction)
	{
		return;
	}

	PieceAction->UnInit();

	PieceActions.Remove(PieceAction);
}

void UCookieLandPiece::RemovePieceActionById(int InId)
{
	UCookieLandBasePieceAction* PieceAction = GetPieceActionById(InId);
	if (!PieceAction)
	{
		return;
	}

	if (PieceAction->GetActiving())
	{
		PieceAction->Finish();
	}
	PieceAction->UnInit();

	PieceActions.Remove(PieceAction);
}

void UCookieLandPiece::ClearPieceActions()
{
	TArray<int> PieceActionIds;
	for (int Index = 0; Index < PieceActions.Num(); ++Index)
	{
		UCookieLandBasePieceAction* PieceAction = PieceActions[Index];
		if (PieceAction)
		{
			PieceActionIds.Add(PieceAction->GetId());
		}
	}

	for (int Index = 0; Index < PieceActionIds.Num(); ++Index)
	{
		int PieceActionId = PieceActionIds[Index];
		RemovePieceActionById(PieceActionId);
	}
}

const UCookieLandBasePieceAction* UCookieLandPiece::GetConstPieceActionByType(TSubclassOf<UCookieLandBasePieceAction> PieceActionClassType)
{
	return GetPieceActionByType(PieceActionClassType);
}

const UCookieLandBasePieceAction* UCookieLandPiece::GetConstPieceActionById(int InId)
{
	return GetPieceActionById(InId);
}

UCookieLandBasePieceAction* UCookieLandPiece::GetPieceActionByType(TSubclassOf<UCookieLandBasePieceAction> PieceActionClassType)
{
	if (PieceActionClassType == nullptr)
	{
		return nullptr;
	}

	for (int Index = 0; Index < PieceActions.Num(); ++Index)
	{
		UCookieLandBasePieceAction* PieceAction = PieceActions[Index];
		if (PieceAction && PieceAction->GetClass() == PieceActionClassType)
		{
			return PieceAction;
		}
	}
	return nullptr;
}

UCookieLandBasePieceAction* UCookieLandPiece::GetPieceActionById(int InId)
{
	for (int Index = 0; Index < PieceActions.Num(); ++Index)
	{
		UCookieLandBasePieceAction* PieceAction = PieceActions[Index];
		if (PieceAction && PieceAction->GetId() == InId)
		{
			return PieceAction;
		}
	}
	return nullptr;
}

void UCookieLandPiece::AddActiveGameplayTag(FGameplayTag InGameplayTag)
{
	ActivingTagContainer.AddTag(InGameplayTag);
}

void UCookieLandPiece::RemoveActiveGameplayTag(FGameplayTag InGameplayTag)
{
	ActivingTagContainer.RemoveTag(InGameplayTag);
}

bool UCookieLandPiece::HasActiveGameplayTag(FGameplayTag InGameplayTag)
{
	return ActivingTagContainer.HasTag(InGameplayTag);
}


#pragma endregion


#pragma region UCookieLandPieceLibrary

void UCookieLandPieceLibrary::AddActiveGameplayTag(UCookieLandPiece* InPiece, FGameplayTag InGameplayTag)
{
	if (!InPiece)
	{
		return;
	}

	InPiece->AddActiveGameplayTag(InGameplayTag);
}
void UCookieLandPieceLibrary::RemoveActiveGameplayTag(UCookieLandPiece* InPiece, FGameplayTag InGameplayTag)
{
	if (!InPiece)
	{
		return;
	}

	InPiece->RemoveActiveGameplayTag(InGameplayTag);
}

bool UCookieLandPieceLibrary::HasActiveGameplayTag(UCookieLandPiece* InPiece, FGameplayTag InGameplayTag)
{
	if (!InPiece)
	{
		return false;
	}

	bool bHas = InPiece->HasActiveGameplayTag(InGameplayTag);
	return bHas;
}
#pragma endregion
