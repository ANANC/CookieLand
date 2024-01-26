// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandEditorWidget.h"
#include "UObject/Object.h"
#include "LandEditorPieceWidget.generated.h"

/**
 * 
 */
UCLASS()
class LANDEDITORTOOL_API ULandEditorPieceWidget : public UObject,public ILandEditorChildWidgetInterface
{
	GENERATED_BODY()

	
	ULandEditorPieceWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	TSharedPtr<SVerticalBox> RootWidget;
	
	TSharedPtr<SVerticalBox> SelectPieceWidget;
	TSharedPtr<SVerticalBox> BottomWidget;
	TSharedPtr<SVerticalBox> Bottom_FloorWidget;
	TSharedPtr<SVerticalBox> Bottom_SliderWidget;
	
	ILandEditorParentWidgetInterface* ParentWidget;
	
	UPROPERTY()
	class ULandEditorWidgetInfo* GlobalWidgetInfo;

	UPROPERTY()
	class ULandEditorCheckerboardWidget* CheckerboardWidget;

	bool IsAutoCreate{true};

public:
	virtual void SetParentWidget(ILandEditorParentWidgetInterface* widget) override{ParentWidget = widget;}
	virtual void SetGlobalWidgetInfo(class ULandEditorWidgetInfo* widgetInfo) override;
	virtual FName GetTabId() override {return "LandEditorWidget.PieceView";}
	virtual FString GetTitleName()  override {return "PieceView";}
	
	virtual TSharedRef<SVerticalBox> GetWidget() override;

	virtual bool EnableDraw() override;
	
	virtual void DrawView() override;

private:
	UFUNCTION()
	void LandEditorCheckerboardWidgetSelectChangeEventCallback();

	void DrawSelectPiece();

	void DrawEmptyPiece();

	void DrawValidPiece(class UPieceBaseConfigData* piece);

	void CreatePiece(FPieceLocation location);
	
	void DeletePiece(FPieceLocation location);
	
	void DrawBottom();
	
	void DrawFloor();

	void DrawSlider();
};
