// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookieLand/Map/CookieLandMapTypes.h"
#include "Delegates/DelegateCombinations.h"
#include "CookieLandPerceptualObjectSubsystem.generated.h"

UCLASS()
class COOKIELAND_API UCookieLandPerceptualObject : public UObject
{
	GENERATED_BODY()

public:
	// 感知者Id
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Id = -1;

	// 感知者类型
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName PerceptualObjectType = "";

	// 能否被感知
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEnablePerceptual = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandLocation PieceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECookieLandPieceOrientation PieceOrientation;

	// 感知配置信息
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCookieLandPerceptualObjectPerceptionInfo PerceptionInfo;

	// 是否在感知中
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPerpetuating = false;
public:
	void Copy(const UCookieLandPerceptualObject* Source)
	{
		if (!Source) { return; }
		Id = Source->Id;
		PerceptualObjectType = Source->PerceptualObjectType;
		bEnablePerceptual = Source->bEnablePerceptual;
		PieceLocation = Source->PieceLocation;
		PieceOrientation = Source->PieceOrientation;
		PerceptionInfo = Source->PerceptionInfo;
		bPerpetuating = Source->bPerpetuating;
	}

	FCookieLandPieceLocator GetLocator() const
	{
		return FCookieLandPieceLocator(PieceLocation, PieceOrientation);
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPerceptualObjectLocatorChangeEvent, int, Id, FCookieLandPieceLocator, OldLocator, FCookieLandPieceLocator, NewLocator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPerceptualObjectPerceptionInfoUpateEvent, int, Id);
DECLARE_DELEGATE_RetVal_OneParam(FCookieLandOrientationLinkInfo, FPerceptualObjectFindLinkInfoEvent, FCookieLandPieceLocator);

UCLASS()
class COOKIELAND_API UCookieLandPerceptualObjectSubsystem : public UObject
{
	GENERATED_BODY()

public:
	FPerceptualObjectLocatorChangeEvent MainPerceptualObjectLocatorChangeEvent; // 主感知对象的坐标更新
	FPerceptualObjectLocatorChangeEvent PerceptualObjectLocatorChangeEvent; // 感知对象的坐标更新

	FPerceptualObjectPerceptionInfoUpateEvent PerceptualObjectPerceptionInfoUpateEvent; // 感知对象的感知信息更新

	FPerceptualObjectFindLinkInfoEvent PerceptualObjectFindForceLinkInfoEvent; // 获取感知对象所在的强制连接信息
protected:

	UPROPERTY()
	TArray< UCookieLandPerceptualObject*> PerceptualObjects;

	UPROPERTY()
	UCookieLandPerceptualObject* MainPerceptualObject = nullptr;

	UPROPERTY()
	TArray<FCookieLandPieceLocator> PassivePerceptualObjectLocators;//【无视坐标】全部能感知的感知者坐标

	UPROPERTY()
	TArray<int> PerceptualingPerceptualObjectIds;//【基于坐标】全部能感知的感知者Id

	UPROPERTY()
	TArray<FCookieLandPieceLocator> PerceptualingPerceptualObjectLocators;//【基于坐标】全部能感知的感知者坐标

	int AutoId = 0;

	// 地图视角
	ECookieLandMapAngleViewType MapAngleViewType = ECookieLandMapAngleViewType::ForwardAndRight;

public:
	// 添加感知对象
	int AddPerceptualObject(bool bMainPerceptualObject,FName InPerceptualObjectType, bool bInEnablePerceptual);

	// 移除感知对象
	void RemovePerceptualObject(int Id);

	// 更新感知对象位置
	void UpdatePerceptualObjectLocation(int Id, FCookieLandLocation PieceLocation);

	// 更新感知对象坐标
	void UpdatePerceptualObjectLocator(int Id,FCookieLandPieceLocator PieceLocator);

	// 更新感知对象能否被感知
	void UpdatePerceptualObjectEnablePerceptual(int Id, bool bInEnablePerceptual);

	// 更新感知对象的感知信息
	void UpdatePerceptualObjectPerceptionInfo(int Id, FCookieLandPerceptualObjectPerceptionInfo InPerceptionInfo);

	// 获取主感知对象
	const UCookieLandPerceptualObject* GetMainPerceptualObject();

	// 获取感知对象
	const UCookieLandPerceptualObject* GetPerceptualObject(int Id);

	// 是否有感知者站着特定位置
	bool HasPerceptualObjectStandByTargetLocation(FCookieLandPieceLocator Locator);

	// 获取主感知对象的坐标信息
	bool GetMainCurrentLocator(FCookieLandPieceLocator& MainLocator);

	// 设置地图视角类型
	void SetMapAngleViewType(ECookieLandMapAngleViewType InMapAngleViewType);

	// 获取地图视角类型
	ECookieLandMapAngleViewType GetMapAngleViewType();

	// 主感知者获取能否感知到感知者
	bool GetMainPerceptualObjectEnablePerceivePassiveePerceptualObject(const UCookieLandPerceptualObject* PerceptualObject);

	// 获取能否感知到感知者
	bool GetEnablePerceivePassivePerceptualObject(
		const FCookieLandPerceptualObjectPerceptionInfo& CenterPerceptionInfo,
		const FCookieLandPerceptualObjectPerceptionInfo& PerceptualObjectPerceptionInfo,
		FCookieLandPieceLocator CenterLocator,
		FCookieLandPieceLocator PerceptualObjectLocator);

	// 主感知者获取能否感知到坐标
	bool GetMainPerceptualObjectEnablePerceiveLocator(FCookieLandPieceLocator PerceptualObjectLocator);
	
	// 获取能否感知到坐标
	bool GetEnablePerceiveLocator(const FCookieLandPerceptualObjectPerceptionInfo& CenterPerceptionInfo, FCookieLandPieceLocator CenterLocator, FCookieLandPieceLocator PerceptualObjectLocator);

protected:
	// 创建感知对象
	UCookieLandPerceptualObject* CreatePerceptualObject();

	// 寻找感知对象
	UCookieLandPerceptualObject* FindPerceptualObject(int Id);

	// 更新被动感知对象坐标信息（除主对象和不可感知对象外）
	void UpdatePassivePerceptualObjectLocators();

	// 从表里获取感知信息
	bool GetPerceptionInfoFromTable(FName PerceptualObjectType, FCookieLandPerceptualObjectPerceptionInfo& PerceptionInfo);
};
