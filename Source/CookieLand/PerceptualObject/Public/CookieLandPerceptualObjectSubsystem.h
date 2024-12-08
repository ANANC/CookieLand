// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
#include "Runtime/Core/Public/Delegates/DelegateCombinations.h"
#include "CookieLandPerceptualObjectSubsystem.generated.h"

UCLASS()
class UCookieLandPerceptualObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Id = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName PerceptualObjectType = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEnablePerceptual = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation PieceOrientation;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPerceptualObjectLocatorChangeEvent, int, Id, FCookieLandPieceLocator, OldLocator, FCookieLandPieceLocator, NewLocator);

UCLASS()
class COOKIELAND_API UCookieLandPerceptualObjectSubsystem : public UObject
{
	GENERATED_BODY()

public:
	FPerceptualObjectLocatorChangeEvent MainPerceptualObjectLocatorChangeEvent;
	FPerceptualObjectLocatorChangeEvent PerceptualObjectLocatorChangeEvent;

protected:

	UPROPERTY()
	TArray< UCookieLandPerceptualObject*> PerceptualObjects;

	UPROPERTY()
	UCookieLandPerceptualObject* MainPerceptualObject = nullptr;

	UPROPERTY()
	TArray<FCookieLandPieceLocator> PassivePerceptualObjectLocators;

	int AutoId = 0;

	// 地图视角
	ECookieLandMapAngleViewType MapAngleViewType = ECookieLandMapAngleViewType::ForwardAndRight;

public:
	// 添加感知对象
	int AddPerceptualObject(bool bMainPerceptualObject,FName InPerceptualObjectType, bool bInEnablePerceptual);

	// 移除感知对象
	void RemovePerceptualObject(int Id);

	// 更新感知对象坐标
	void UpdatePerceptualObjectLocator(int Id,FCookieLandPieceLocator PieceLocator);

	// 更新感知对象能否被感知
	void UpdatePerceptualObjectEnablePerceptual(int Id, bool bInEnablePerceptual);

	// 获取主感知对象
	const UCookieLandPerceptualObject* GetMainPerceptualObject();

	// 获取主感知对象的坐标信息
	bool GetMainCurrentLocator(FCookieLandPieceLocator& MainLocator);

	// 设置地图视角类型
	void SetMapAngleViewType(ECookieLandMapAngleViewType InMapAngleViewType);

	// 获取地图视角类型
	ECookieLandMapAngleViewType GetMapAngleViewType();

protected:
	// 创建感知对象
	UCookieLandPerceptualObject* CreatePerceptualObject();

	// 寻找感知对象
	UCookieLandPerceptualObject* FindPerceptualObject(int Id);

	// 更新被动感知对象坐标信息（除主对象和不可感知对象外）
	void UpdatePassivePerceptualObjectLocators();
};
