// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandActionTypes.h"
#include "CookieLandBasePieceAction.generated.h"


class UCookieLandBasePieceAction;
class UCookieLandPiece;
class ACookieLandMapBuildActor;
class UCookieLandMapBuilder;
class UCookieLandBaseAnimTask;
class ACookieLandBaseCueActor;


UCLASS(Blueprintable,BlueprintType)
class COOKIELAND_API UCookieLandBasePieceAction : public UObject
{
	GENERATED_BODY()
	
protected:

	int Id = -1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UCookieLandBasePieceActionData* BaseData = nullptr;

	UPROPERTY()
	TObjectPtr< UCookieLandPiece> Piece;

	UPROPERTY()
	TArray< TObjectPtr<UCookieLandBaseAnimTask>> Tasks;

	UPROPERTY()
	TArray< TObjectPtr<ACookieLandBaseCueActor>> CueActors;

	int AutoTaskId = 0;

public:
	// 时机： Init -> Active -> Finish -> UnInit

	// 初始化
	virtual void Init(int InId, UCookieLandPiece* InPiece,UCookieLandBasePieceActionData* InData);
	// 反初始化
	virtual void UnInit();

	// 激活
	virtual void Active();
	// 结束
	virtual void Finish();

	// -- base data --
public:
	int GetId() const;

	UCookieLandPiece* GetPiece();

	// 获取地图构建Actor
	ACookieLandMapBuildActor* GetMapBuildActor();

	UCookieLandMapBuilder* GetMapBuilder();

	// 获取配置数据
	const UCookieLandBasePieceActionData* GetData() const;

protected:
	// 设置配置数据
	virtual void SetData(UCookieLandBasePieceActionData* InData);


	// -- Task --
public:
	// 添加Task
	void AddTask(UCookieLandBaseAnimTask* InTask);

	// 通过Id移除Task
	void RemoveTaskById(int InTaskId);

	// -- CueActor --
public:
	
	// 创建CueActor 通过tag
	static ACookieLandBaseCueActor* CreateCueActor(UCookieLandBasePieceAction* InPieceAction, FGameplayTag CueActorTag, UCookieLandBaseCueActorData* InBaseData = nullptr);

	// 创建CueActor 通过具体参数
	static ACookieLandBaseCueActor* CreateCueActor(UCookieLandBasePieceAction* InPieceAction,TSubclassOf< ACookieLandBaseCueActor> CueActorType, UCookieLandBaseCueActorData* InBaseData);

	// 添加CueActor实例
	void AddCueActor(ACookieLandBaseCueActor* InCueActor,UCookieLandBaseCueActorData* InCueActorData);

	// 销毁CueActor
	static void DestroyCueActor(UCookieLandBasePieceAction* InPieceAction, ACookieLandBaseCueActor* InCueActor);

	// 移除CueActor实例
	void RemoveCueActor(ACookieLandBaseCueActor* InCueActor);

protected:
	// 获取CueActor数据
	bool GetCueActorDataByTag(FGameplayTag CueActorTag, FCueActorData& OutCueActorData);
};
