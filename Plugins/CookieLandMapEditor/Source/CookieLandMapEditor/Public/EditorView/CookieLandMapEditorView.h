// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
#include "CookieLandMapEditorView.generated.h"

class SVerticalBox;
class SBorder;
class ACookieLandMapBuildActor;
class UCookieLandPiece;
class UCookieLandMapBuilder;
class UCookieLandMapActorGather;

UENUM(BlueprintType)
enum class ECookieLandMapEditorContentTyle : uint8
{
	MapBuildView
};

UCLASS()
class UCookieLandMapEditor_Checkerboard : public UObject
{
	GENERATED_BODY()

public:

	FVector LandSize{ 100,100,0 };
	FVector DrawStartLocation{ 0,0,0 };
	FVector CurCenterSlider{ 0.5,0.5,0 };
	FVector DrawSize{ 30,20,0 };

	FSlateColor SelectColor;
	FSlateColor BgHasDataColor;
	FSlateColor BgEmptyColor;

	int size_x = 25;
	int size_y = 25;

	TMap< FVector, TSharedPtr<SBorder>> Location2Border;
	TMap< ECookieLandPieceOrientation, TSharedPtr<SBorder>>Orientation2Border;

	TSharedPtr<SVerticalBox> SelectPieceContextVerticalBox;
};


UCLASS()
class UCookieLandMapEditor_SelectMapCube : public UObject
{
	GENERATED_BODY()

public:

	FVector SelectLocation{15,10,0};
	ECookieLandPieceOrientation SelectOrientation;

	TSharedPtr<SVerticalBox> SelectContextVerticalBox;
};


UCLASS()
class UCookieLandMapEditorView : public UObject
{
	GENERATED_BODY()
	
public:
	void Init();

	TSharedRef<SDockTab> Draw();

public:
	UPROPERTY()
	TObjectPtr<ACookieLandMapBuildActor> MapBuildActor;

	UPROPERTY()
	UCookieLandMapEditor_Checkerboard* Checkerboard;

	UPROPERTY()
	UCookieLandMapEditor_SelectMapCube* SelectMapCube;

	ECookieLandMapEditorContentTyle MapEditorContentTyle;
	TSharedPtr<SVerticalBox> ContentViewVerticalBox;

protected:

	// 渲染 标题
	TSharedPtr<SVerticalBox> Draw_TitleButtons();

	// 更新 内容页
	void DrawUpdateContentViewVerticalBox();

	// 渲染 地图构建页
	TSharedPtr<SVerticalBox> Draw_MpaBuildView();

	// 渲染 地图构建页-棋盘
	TSharedPtr<SVerticalBox> Draw_Checkerboard();

	// 更新 地图构建页-棋盘-棋子
	void DrawUpdateChessPiece(FCookieLandLocation PieceLocation);

	// 渲染 地图构建页-棋盘-棋子 内容页
	TSharedPtr<SHorizontalBox> Draw_ChessPieceContent(FCookieLandLocation PieceLocation);

	// 更新 地图构建页-选中详情
	void DrawUpdateSelectContext();

	// 渲染 地图构建页-选中详情-方位
	TSharedPtr<SVerticalBox> Draw_Orientation();

	// 更新 地图构建页-选中详情-方位块
	void DrawUpdateSelectOrientation(ECookieLandPieceOrientation Orientation);

	// 更新 地图构建页-选中详情-棋子
	void DrawUpdateSelectPieceContext();

	// 渲染 地图构造页-选中详情-强制连接
	TSharedPtr<SVerticalBox> Draw_ForceLinkContext();

	// 渲染 编辑器操作页
	TSharedPtr<SVerticalBox> Draw_ViewControlContext();

protected:
	// 是否地图坐标信息上有立方体信息
	bool HasMapBuildDataByLocation(FCookieLandLocation PieceLocation);

	// 是否地图坐标信息上有地块信息
	bool HasPieceDataByLocation(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation);

	// 获取地块
	UCookieLandPiece* GetPiece(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation);

protected:

	// 棋子点击回调
	void ChessPieceButtonClickCallback(FCookieLandLocation PieceLocation);

	// 方位点击回调
	void OrientationButtonClickCallback(ECookieLandPieceOrientation PieceOrientation);

	// 创建地块回调
	void CreatePieceButtonClickCallback();

	// 删除地块回调
	void DeletePieceButtonClickCallback();

	// 地块实例类型改变回调
	void PieceActorTypeChangeCallback(const FAssetData& AssetData);

	// 切换层级回调
	void ChangeFloorButtonClickCallback(int InAddValue);

	// 删除当前层的全部地块点击回调
	void DeleteSelectFloorAllPieceButtonClickCallback();

	// 删除全部地块点击回调
	void DeleteAllPieceButtonClickCallback();

	// 删除强制连接
	void DeleteForceLink(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation);
protected:

	// 将地形数据保存到da
	void SaveMapBuildDataToDataAsset();
};
