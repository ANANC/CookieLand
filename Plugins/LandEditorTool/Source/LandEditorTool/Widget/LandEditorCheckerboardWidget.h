// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandEditorWidget.h"
#include "UObject/Object.h"
#include "LandEditorCheckerboardWidget.generated.h"

class SVerticalBox;
class SHorizontalBox;
class SBorder;

DECLARE_DELEGATE( FLandEditorCheckerboardWidgetSelectChangeEvent )

UCLASS()
class ULandEditorCheckerboardWidgetInfo : public UObject
{
	GENERATED_BODY()

public:
	int CurFloor{0};
	FVector LandSize{100,100,0};
	FVector DrawStartLocation{0,0,0};
	FVector CurCenterSlider{0.5,0.5,0};
	FVector DrawSize{30,20,0};
	FVector SelectLocation{15,10,0};
};

UCLASS()
class LANDEDITORTOOL_API ULandEditorCheckerboardWidget : public UObject,public ILandEditorChildWidgetInterface
{
	GENERATED_BODY()

	
	ULandEditorCheckerboardWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	FLandEditorCheckerboardWidgetSelectChangeEvent LandEditorCheckerboardWidgetSelectChangeEvent;

	UPROPERTY()
	class ULandEditorCheckerboardWidgetInfo* CheckerboardWidgetInfo;
	
protected:
	TSharedPtr<SVerticalBox> RootWidget;
	TMap<FVector,TSharedPtr<SBorder>> PieceBoarders;
	
	ILandEditorParentWidgetInterface* ParentWidget;
	
	UPROPERTY()
	class ULandEditorWidgetInfo* GlobalWidgetInfo;

private:
	int size_x = 25;
	int size_y = 25;
	
public:
	virtual void SetParentWidget(ILandEditorParentWidgetInterface* widget) override{ParentWidget = widget;}
	virtual void SetGlobalWidgetInfo(class ULandEditorWidgetInfo* widgetInfo) override{GlobalWidgetInfo = widgetInfo;}
	virtual FName GetTabId() override {return "LandEditorWidget.CheckerboardView";}
	virtual FString GetTitleName()  override {return "CheckerboardView";}
	
	virtual TSharedRef<SVerticalBox> GetWidget() override;

	virtual bool EnableDraw() override;
	
	virtual void DrawView() override;

	FVector ChangeToLogicLocation(FPieceLocation location);
	FPieceLocation ChangeToGameLocation(FVector location);

	void SelectLocation(FPieceLocation location);
	void UpdateLocation(int id);
	void UpdateLocation(FPieceLocation location);
	
private:
	TSharedPtr<SHorizontalBox> GetButtonBoxContent(class UPieceBaseConfigData* pieceConfigData);

	void ButtonClickCallback(FPieceLocation location);

};
