// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceAction_Rotation.h"

#include "CookieLand/Character/BaseCharacter.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/Piece/BasePiece.h"
#include "CookieLand/Piece/BasePieceLand.h"
#include "CookieLand/Piece/Component/PieceActionStateOComponent.h"
#include "CookieLand/Piece/Component/PieceLandComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void UPieceAction_Rotation::_Init()
{
	if(!ConfigData)
	{
		SetIsFinish(true);
		return;
	}

	IsTriggering = false;

	RuntimeInfo = NewObject<UPieceActionRunTimeInfo_Rotation>();
	RuntimeInfo->RotationAngle = GetRotationAngle();
	RuntimeInfo->AngleType = ConfigData->RotationAngle;
	
	UPieceActionStateOComponent* actionStateOComponent = Piece->GetActionStateOComponent();
	FAddPieceActionStateRequest addRequest(EPieceActionState::Rotation,true,true);
	actionStateOComponent->AddState(addRequest,RuntimeInfo);
}

void UPieceAction_Rotation::_UnInit()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		pieceLandComponent->MoveToNextPieceEvent.RemoveAll(this);
	}

	UPieceActionStateOComponent* actionStateOComponent = Piece->GetActionStateOComponent();
	actionStateOComponent->RemoveState(EPieceActionState::Rotation);

	if(GetWorld() && RotationIntervalTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RotationIntervalTimerHandle);
		RotationIntervalTimerHandle.Invalidate();
	}
	
	RotationNumber = 0;
	LastRotationTime = 0;
}

void UPieceAction_Rotation::SetData(class UPieceBaseActionConfigData* data)
{
	Super::SetData(data);
	ConfigData = Cast<UPieceActionConfigData_Rotation>(data);
}

void UPieceAction_Rotation::Execute()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	UPieceLandComponent* pieceLandComponent = nullptr;
	if(mainCharacter)
	{
		pieceLandComponent = mainCharacter->GetPieceLandComponent();
	}
	
	switch (ConfigData->TriggerType)
	{
	case EPieceActionRotationTriggerType::AutoTrigger:
		TriggerRotation();
		break;
	case EPieceActionRotationTriggerType::EnterPiece:
		if(!pieceLandComponent)
		{
			return;
		}
		pieceLandComponent->MoveToNextPieceEvent.AddDynamic(this,&UPieceAction_Rotation::MoveToNextPieceEventCallback);
		
		int characterStandPieceId;
		Piece->GetOwnLand()->GetPieceIdByLocation( pieceLandComponent->GetCurLocation(),characterStandPieceId);
		if(characterStandPieceId == -1)
		{
			characterStandPieceId = Piece->GetOwnLand()->GetInitialPieceId();
		}

		if(characterStandPieceId == PieceId)
		{
			TriggerRotation();
		}
		break;
	case EPieceActionRotationTriggerType::ExitPiece:
	case EPieceActionRotationTriggerType::PlayerMove:
		if(!pieceLandComponent)
		{
			return;
		}
		pieceLandComponent->MoveToNextPieceEvent.AddDynamic(this,&UPieceAction_Rotation::MoveToNextPieceEventCallback);
		break;
	default:
		break;
	}
}

void UPieceAction_Rotation::MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation)
{
	if(IsTriggering)
	{
		return;
	}
	
	int characterLastPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation(oldLocation,characterLastPieceId);
	
	int characterStandPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation(newLocation,characterStandPieceId);

	bool enableTrigger;
	switch (ConfigData->TriggerType)
	{
	case EPieceActionRotationTriggerType::EnterPiece:
		enableTrigger = characterStandPieceId == PieceId;
		break;
	case EPieceActionRotationTriggerType::ExitPiece:
		enableTrigger = characterLastPieceId == PieceId;
		break;
	case EPieceActionRotationTriggerType::PlayerMove:
		enableTrigger = true;
		break;
	default:
		enableTrigger = false;
		break;
	}

	if(enableTrigger)
	{
		TriggerRotation();
	}
}

void UPieceAction_Rotation::TriggerRotation()
{
	IsTriggering = true;

	bool enableNextRotation;
	switch (ConfigData->RotationCount)
	{
	case EPieceActionRotationCountType::Single:
		enableNextRotation = false;
		break;
	case EPieceActionRotationCountType::Number:
		RotationNumber+=1;
		enableNextRotation = RotationNumber<ConfigData->RotationNumber;
		break;
	case EPieceActionRotationCountType::ContinueTime:
		if(LastRotationTime == 0)
		{
			LastRotationTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);
		}
		enableNextRotation = (UKismetSystemLibrary::GetGameTimeInSeconds(this) - LastRotationTime) < ConfigData->ContinueTime;
		break;
	case EPieceActionRotationCountType::Continue:
		enableNextRotation = true;
		break;
	default:
		enableNextRotation = false;
		break;
	}

	RotationPiece();

	if(enableNextRotation)
	{
		GetWorld()->GetTimerManager().SetTimer(RotationIntervalTimerHandle,this,&UPieceAction_Rotation::TriggerRotation,ConfigData->IntervalTime,false);
	}
	else
	{
		IsTriggering = false;

		RotationNumber = 0;
		LastRotationTime = 0;
	}
}

void UPieceAction_Rotation::RotationPiece()
{
	Piece->GetActionStateOComponent()->TriggerAction_Rotation(ConfigData->RotationAngle == EPieceActionRotationAngleType::Vertical, GetRotationAngle());
}

float UPieceAction_Rotation::GetRotationAngle()
{
	switch (ConfigData->RotationAngle)
	{
	case EPieceActionRotationAngleType::Clockwise:
		return 90;
	case EPieceActionRotationAngleType::Anticlockwise:
		return -90;
	case EPieceActionRotationAngleType::Horizontal:
		return 180;
	default:
		return 0;
	}
}