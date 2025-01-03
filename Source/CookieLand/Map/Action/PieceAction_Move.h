// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLandBasePieceAction.h"
#include "PieceAction_Move.generated.h"

class UAnimTask_Move;

// 基础行为：移动
UCLASS()
class COOKIELAND_API UPieceAction_Move : public UCookieLandBasePieceAction
{
	GENERATED_BODY()
	
protected:
	virtual void SetData(UCookieLandBasePieceActionData* InData) override;

public:
	virtual void Active() override;
	virtual void Finish() override;

private:
	UPROPERTY()
	UPieceActionData_Move* Data = nullptr;//移动配置数据

	UPROPERTY()
	ACookieLandBaseCueActor* CueActor_Move = nullptr; //移动CueActor
protected:
	UPROPERTY()
	FCookieLandPieceLocator StartLocator;
	UPROPERTY()
	FCookieLandPieceLocator EndLocator;

	UPROPERTY()
	ECookieLandPieceOrientation MoveOrientation;

	UPROPERTY()
	UAnimTask_Move* MoveTask = nullptr;//移动Task
protected:
	//获取移动目标点
	FCookieLandPieceLocator GetMoveTarget();

	// 尝试触发
	void TryTrigger(EPieceActionMoveTriggerType InTriggerType);

	// 执行任务
	void PlayMoveTask();

	//移动Task结束回调
	UFUNCTION()
	virtual void MoveTaskFinishEventCallback();

	// 接收感知者位移
	UFUNCTION()
	virtual void ReceivePerceptualObjectLocatorChangeEventCallback(int InId, FCookieLandPieceLocator OldLocator, FCookieLandPieceLocator NewLocator);
};


// 基础行为：来回移动
UCLASS()
class COOKIELAND_API UPieceAction_MoveBackAndForth : public UPieceAction_Move
{
	GENERATED_BODY()

protected:
	UPieceActionData_MoveBackAndForth* GetMyData();

protected:

	//移动Task结束回调
	virtual void MoveTaskFinishEventCallback() override;
};