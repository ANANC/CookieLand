// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandActionTypes.h"

#pragma region UPieceActionData_Move

void UPieceActionData_Move::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.MemberProperty)
	{
		const FName MemberPropertyName = PropertyChangedEvent.MemberProperty->GetFName();
		if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UPieceActionData_Move, Trigger))
		{
			bActiveImmediately = Trigger == EPieceActionMoveTriggerType::Auto;
		}
	}
}

#pragma endregion