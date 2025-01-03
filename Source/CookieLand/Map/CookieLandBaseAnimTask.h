// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/DelegateCombinations.h"
#include "CookieLandBaseAnimTask.generated.h"
 
class UCookieLandBasePieceAction;
class UCookieLandPiece;
class ACookieLandPieceActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCookieLandAnimTaskEvent);

UCLASS()
class COOKIELAND_API UCookieLandBaseAnimTask : public UObject
{
	GENERATED_BODY()
	
public:
	// 时机： Init -> Active -> Finish -> UnInit

	// 初始化
	virtual void Init(int InId, UCookieLandBasePieceAction* InAction);
	// 反初始化
	virtual void UnInit();

	// 激活
	virtual void Active();
	// 结束
	virtual void Finish();

	// 创建Task
	template<class T>
	static T* NewAnimTask(UCookieLandBasePieceAction* ThisAction)
	{
		check(ThisAction);
		T* MyObj = static_cast<T*>(CreateAnimTask(ThisAction, T::StaticClass()));
		return MyObj;
	}

protected:
	static UCookieLandBaseAnimTask* CreateAnimTask(UCookieLandBasePieceAction* ThisAction,TSubclassOf< UCookieLandBaseAnimTask> TaskType);

public:
	FOnCookieLandAnimTaskEvent ActiveEvent; //激活事件
	FOnCookieLandAnimTaskEvent FinishEvent; //结束事件

protected:
	int Id = -1;

	UPROPERTY()
	TObjectPtr<UCookieLandBasePieceAction> Action = nullptr;

	UPROPERTY()
	TObjectPtr< UCookieLandPiece> Piece = nullptr;

	bool bIsActive = false;
	bool bIsFinish = false;

public:
	int GetId();

	UCookieLandBasePieceAction* GetAction();

	UCookieLandPiece* GetPiece();

	ACookieLandPieceActor* GetPieceActor();

public:
	// 获取是否激活中
	bool GetIsActiving();
};
