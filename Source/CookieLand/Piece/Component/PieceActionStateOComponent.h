// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPieceBaseOComponent.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "UObject/Object.h"
#include "PieceActionStateOComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActionStateUpdateEvent, EPieceActionState, actionState, bool, isAttach);
/**
 * 
 */
UCLASS()
class COOKIELAND_API UPieceActionStateOComponent : public UObject,public IPieceBaseOComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FActionStateUpdateEvent ActionStateUpdateEvent;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	class UBasePiece* Piece;
	
	UPROPERTY(BlueprintReadOnly)
	EPieceActionState MainState{EPieceActionState::Empty};
	
	UPROPERTY()
	TArray<EPieceActionState> AttachStates;

	bool EnableReplaceMainState{true};
	
	UPROPERTY()
	TMap<EPieceActionState,class UPieceBaseActionRunTimeInfo*> ActionStateRunTimeInfos;
	
public:
	virtual void SetPiece(class UBasePiece* piece) override;
	virtual void Init() override;
	virtual void UnInit() override;

public:

	UFUNCTION(BlueprintCallable)
	void AddState(FAddPieceActionStateRequest addRequest,class UPieceBaseActionRunTimeInfo* actionInfo = nullptr);

	UFUNCTION(BlueprintCallable)
	void RemoveState(EPieceActionState actionState);
	
	UFUNCTION(BlueprintCallable)
	bool GetIsAttachState(EPieceActionState actionState);

	template<class T>
	T* GetActionRuntimeInfo(EPieceActionState actionState)
	{
		UPieceBaseActionRunTimeInfo* info = GetActionRuntimeInfoByType(actionState);
		if(info)
		{
			return dynamic_cast<T>(info);
		}
		return nullptr;
	}

	UFUNCTION(BlueprintPure)
	class UPieceBaseActionRunTimeInfo* GetActionRuntimeInfoByType(EPieceActionState actionState);
	
public:
	//掉落
	void TriggerAction_RemindDropOut();
	void TriggerAction_DropOut();

	//旋转
	void TriggerAction_Rotation(bool isVertical,float angle);
};

