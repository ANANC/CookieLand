// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandEditorWidget.h"
#include "UObject/Object.h"
#include "LandEditorDataAssetWidget.generated.h"


class SVerticalBox;

UCLASS()
class LANDEDITORTOOL_API ULandEditorDataAssetWidget : public UObject,public ILandEditorChildWidgetInterface
{
	GENERATED_BODY()

	
	ULandEditorDataAssetWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	TSharedPtr<SVerticalBox> RootWidget;
	
	ILandEditorParentWidgetInterface* ParentWidget;
	
	UPROPERTY()
	class ULandEditorWidgetInfo* GlobalWidgetInfo;
	
public:
	virtual void SetParentWidget(ILandEditorParentWidgetInterface* widget) override{ParentWidget = widget;}
	virtual void SetGlobalWidgetInfo(class ULandEditorWidgetInfo* widgetInfo) override{GlobalWidgetInfo = widgetInfo;}
	virtual FName GetTabId() override {return "LandEditorWidget.DataAssetView";}
	virtual FString GetTitleName()  override {return "DataAssetView";}
	
	virtual TSharedRef<SVerticalBox> GetWidget() override;

	virtual bool EnableDraw() override;
	
	virtual void DrawView() override;

protected:
	void ReLoadDataAsset(const TCHAR* objectPath);

	void CheckLandData();
	void InitAutoPieceId();
};
