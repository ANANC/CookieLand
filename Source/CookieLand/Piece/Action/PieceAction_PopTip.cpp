// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceAction_PopTip.h"

#include "CookieLand/Character/BaseCharacter.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"
#include "CookieLand/Piece/BasePiece.h"
#include "CookieLand/Piece/BasePieceLand.h"
#include "CookieLand/Piece/Component/PieceLandComponent.h"
#include "CookieLand/Piece/PieceLandSystem.h"

int UPieceAction_PopTip::DynamicTipId = 990000;

void UPieceAction_PopTip::_Init()
{
	if(!ConfigData || (ConfigData->TriggerType == EPieceActionPopTipTriggerType::DoNothing && ConfigData->DeleteType == EPieceActionPopTipDeleteType::DoNothing))
	{
		SetIsFinish(true);
		return;
	}
	TriggerNumber = 0;
}

void UPieceAction_PopTip::_UnInit()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		pieceLandComponent->MoveToNextPieceEvent.RemoveAll(this);
	}
}

void UPieceAction_PopTip::SetData(class UPieceBaseActionConfigData* data)
{
	Super::SetData(data);
	ConfigData = Cast<UPieceActionConfigData_PopTip>(data);
}

void UPieceAction_PopTip::Execute()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(!mainCharacter)
	{
		SetIsFinish(true);
		return;
	}

	tipId = ConfigData->TipData.Id;
	if(ConfigData->TriggerType != EPieceActionPopTipTriggerType::DoNothing && ConfigData->DeleteType != EPieceActionPopTipDeleteType::DoNothing && ConfigData->TipData.Id == -1)
	{
		tipId = DynamicTipId;
		DynamicTipId+=1;
	}

	UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();

	int characterStandPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation( pieceLandComponent->GetCurLocation(),characterStandPieceId);
	if(characterStandPieceId == -1)
	{
		characterStandPieceId = Piece->GetOwnLand()->GetInitialPieceId();
	}
	
	int characterLastPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation(pieceLandComponent->GetLastLocation(),characterLastPieceId);
	
	TryDeleteTip(characterLastPieceId,characterStandPieceId);
	TryTriggerTip(characterLastPieceId,characterStandPieceId);

	pieceLandComponent->MoveToNextPieceEvent.AddDynamic(this,&UPieceAction_PopTip::MoveToNextPieceEventCallback);
}

void UPieceAction_PopTip::MoveToNextPieceEventCallback(FPieceLocation oldLocation,FPieceLocation newLocation)
{
	int characterStandPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation(newLocation,characterStandPieceId);
	
	int characterLastPieceId;
	Piece->GetOwnLand()->GetPieceIdByLocation(oldLocation,characterLastPieceId);
	
	TryDeleteTip(characterLastPieceId,characterStandPieceId);
	TryTriggerTip(characterLastPieceId,characterStandPieceId);
}

void UPieceAction_PopTip::TryTriggerTip(int lastPieceId,int standPieceId)
{
	if(ConfigData->IsTriggerOnce && TriggerNumber>0)
	{
		return;
	}
	
	EPieceActionPopTipTriggerType triggerType = ConfigData->TriggerType;

	bool isTrigger;
	switch (triggerType)
	{
	case EPieceActionPopTipTriggerType::EnterPiece:
		isTrigger = standPieceId == PieceId;
		break;
	case EPieceActionPopTipTriggerType::ExitPiece:
		isTrigger = lastPieceId == PieceId;
		break;
	default:
		isTrigger = false;
		break;
	}

	if(isTrigger)
	{
		TriggerNumber+=1;

		FPiecePopTipData tipData(ConfigData->TipData);
		tipData.Id = tipId;
		UCommonFunctionLibrary::GetPieceLandSystem()->PieceLandShowTipEvent.Broadcast(tipData);
	}
}

void UPieceAction_PopTip::TryDeleteTip(int lastPieceId,int standPieceId)
{
	EPieceActionPopTipDeleteType triggerType = ConfigData->DeleteType;
	
	bool isDelete;
	switch (triggerType)
	{
	case EPieceActionPopTipDeleteType::EnterPiece:
		isDelete = standPieceId == PieceId;
		break;
	case EPieceActionPopTipDeleteType::ExitPiece:
		isDelete = lastPieceId == PieceId;
		break;
	default:
		isDelete = false;
		break;
	}

	if(isDelete)
	{
		if(tipId!=-1)
		{
			UCommonFunctionLibrary::GetPieceLandSystem()->PieceLandHideTargetTipEvent.Broadcast(tipId);
		}
		if(ConfigData->IsDeleteAllTypeTip)
		{
			UCommonFunctionLibrary::GetPieceLandSystem()->PieceLandHideAllTipEvent.Broadcast(false,ConfigData->TipData.IsFixType);
		}
		if(ConfigData->IsDeleteAllTip)
		{
			UCommonFunctionLibrary::GetPieceLandSystem()->PieceLandHideAllTipEvent.Broadcast(true,ConfigData->TipData.IsFixType);
		}
	}
}