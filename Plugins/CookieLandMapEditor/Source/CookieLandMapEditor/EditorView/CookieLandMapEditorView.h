// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CookieLandMapTypes.h"
#include "CookieLandMapEditorView.generated.h"

class SVerticalBox;
class SBorder;
class ACookieLandMapBuildActor;
class UCookieLandPiece;
class UCookieLandMapBuilder;
class UCookieLandMapActorGather;
class UCookieLandPerceptualObject;

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

	int size_x = 26;
	int size_y = 26;

	TMap< FVector, TSharedPtr<SVerticalBox>> Location2VerticalBox;
	TMap< ECookieLandPieceOrientation, TSharedPtr<SVerticalBox>>Orientation2VerticalBox;


};


UCLASS()
class UCookieLandMapEditor_SelectMapCube : public UObject
{
	GENERATED_BODY()

public:

	FVector SelectLocation{15,10,0};
	ECookieLandPieceOrientation SelectOrientation;

	TSharedPtr<SVerticalBox> SelectContextVerticalBox;
	TSharedPtr<SVerticalBox> SelectPieceContextVerticalBox;
	TSharedPtr<SVerticalBox> SelectPieceDetailContextVerticalBox;

	FText ForceLineNumberText;

	TSharedPtr < TStructOnScope<FCookieLandPieceBuildInfo>> SelectPieceBuildInfoParameterValue;
};


UCLASS()
class UCookieLandMapEditor_PerceptualObject : public UObject
{
	GENERATED_BODY()

public:
	TSharedPtr<SVerticalBox> MainPerceptualObjectVerticalBox;

	UPROPERTY()
	UCookieLandPerceptualObject* MainPerceptualObjectEditor;

	FName MainPerceptualObjectType = "Default";
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

	UPROPERTY()
	UCookieLandMapEditor_PerceptualObject* PerceptualObject;

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

	// 渲染 编辑器操作页
	TSharedPtr<SVerticalBox> Draw_ViewControlContext();

	// 更新 地图构建页-选中详情
	void DrawUpdateSelectContext();

	// 渲染 地图构建页-选中详情-方位
	TSharedPtr<SVerticalBox> Draw_Orientation();

	// 更新 地图构建页-选中详情-方位块
	void DrawUpdateSelectOrientation(ECookieLandPieceOrientation Orientation);

	// 更新 地图构建页-选中详情
	void DrawUpdateSelect();

	// 更新 地图构建页-选中详情-棋子
	void DrawUpdateSelectPieceContext();

	// 更新 地图构建页-选中详情-棋子配置
	void DrawUpdateSelectPieceDetailContext();

	// 渲染 地图构建页-选中详情-强制连接
	TSharedPtr<SVerticalBox> Draw_ForceLinkContext();

	// 渲染 地图构建页-感知者
	TSharedPtr<SVerticalBox> Draw_PerceptualObjectContext();
	
	// 更新 地图构建页-感知者-主感知
	void DrawUpdateMainPerceptualObject();

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

	// 设置为初始位置点击回调
	void SetToInitialLocationButtonClickCallback();

	// 设置MainPerceptualObject站立在该地块点击回调
	void SetMainPerceptualObjectStandHereButtonClickCallback();

	// 地块实例类型改变回调
	void PieceActorTypeChangeCallback(const FAssetData& AssetData);

	// 切换层级回调
	void ChangeFloorButtonClickCallback(int InAddValue);

	// 删除当前层的全部地块点击回调
	void DeleteSelectFloorAllPieceButtonClickCallback();

	// 删除全部地块点击回调
	void DeleteAllPieceButtonClickCallback();

	// 更新强制连接回调
	void UpdateForceLineNumberButtonClickCallback();

	// 删除强制连接回调
	void DeleteForceLinkButtonClickCallback();

	// PieceBuildInfo属性修改回调
	void PieceBuildInfoOnFinishedChangingPropertiesCallback(const FPropertyChangedEvent& PropertyChangedEvent);

	// MainPerceptualObject属性修改回调
	void MainPerceptualObjectEditorChangePropertiesCallback(const FPropertyChangedEvent& PropertyChangedEvent);

	// 创建MainPerceptualObject点击回调
	void CreateMainPerceptualObjectButtonClickCallback();

protected:

	// 将地形数据保存到da
	void SaveMapBuildDataToDataAsset();
};
