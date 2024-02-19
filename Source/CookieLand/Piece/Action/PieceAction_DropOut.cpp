// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceAction_DropOut.h"

#include "CookieLand/Character/BaseCharacter.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/Piece/BasePiece.h"
#include "CookieLand/Piece/BasePieceLand.h"
#include "CookieLand/Piece/PieceLandComponent.h"

void UPieceAction_DropOut::_Init()
{
	if(!ConfigData)
	{
		SetIsFinish(true);
		return;
	}
}

void UPieceAction_DropOut::_UnInit()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(GetWorld());
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		pieceLandComponent->MoveToNextPieceEvent.RemoveAll(this);
	}
}

void UPieceAction_DropOut::SetData(class UPieceBaseActionConfigData* data)
{
	Super::SetData(data);
	ConfigData = Cast<UPieceActionConfigData_DropOut>(data);
}

void UPieceAction_DropOut::Execute()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(GetWorld());
	if(!mainCharacter)
	{
		SetIsFinish(true);
		return;
	}

	UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();

	int characterStandPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation( pieceLandComponent->GetCurLocation(),characterStandPieceId);
	if(characterStandPieceId == -1)
	{
		characterStandPieceId = Piece->GetOwnLand()->GetInitialPieceId();
	}
	
	TryTriggerTip(characterStandPieceId);

	pieceLandComponent->MoveToNextPieceEvent.AddDynamic(this,&UPieceAction_DropOut::MoveToNextPieceEventCallback);
}

void UPieceAction_DropOut::MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation)
{
	int characterStandPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation(newLocation,characterStandPieceId);
	
	TryTriggerTip(characterStandPieceId);
}

void UPieceAction_DropOut::TryTriggerTip(int standPieceId)
{
	if(DelayTimerHandle.IsValid())
	{
		return;
	}

	bool isTrigger;
	switch (ConfigData->TriggerType)
	{
	case EPieceActionDropOutTriggerType::EnterPiece:
		isTrigger = standPieceId == PieceId;
		break;
	default:
		isTrigger = false;
		break;
	}

	if(!isTrigger)
	{
		return;
	}
	
	if(!GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle,this,&UPieceAction_DropOut::TriggerDropOut,ConfigData->DelayTime,false);
}

void UPieceAction_DropOut::TriggerDropOut()
{
	
}