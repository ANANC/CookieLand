// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLand/Map/Public/CookieLandPiece.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"
#include "Components/BoxComponent.h"
#include "CookieLand/Map/Public/CookieLandMapShowDirector.h"
#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectComponent.h"
#include "CookieLand/Map/Public/CookieLandBasePieceAction.h"

#pragma region ACookieLandPieceActor

ACookieLandPieceActor::ACookieLandPieceActor(const FObjectInitializer& Initializer) :
	Super(Initializer)
{
	EnterBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EnterBox"));

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


#pragma endregion

#pragma region UCookieLandPiece

void UCookieLandPiece::Init(FCookieLandPieceBuildInfo InBuildInfo)
{
	bInit = true;
	BuildInfo = InBuildInfo;
	PieceLocation = BuildInfo.PieceLocation;
	PieceOrientation = BuildInfo.PieceOrientation;
	BaseAction = BuildInfo.BaseAction;
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
}

void UCookieLandPiece::SetPieceActor(ACookieLandPieceActor* Instance)
{
	PieceActor = Instance;
}

void UCookieLandPiece::SetPieceActorType(TSubclassOf< ACookieLandPieceActor> PieceActorType)
{
	BuildInfo.PieceActorType = PieceActorType;
}

void UCookieLandPiece::SetBaseAction(FCookieLandPieceBaseAction InBaseAction)
{
	BaseAction = InBaseAction;
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

FCookieLandPieceBaseAction UCookieLandPiece::GetBaseAction()
{
	return BaseAction;
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

#pragma endregion