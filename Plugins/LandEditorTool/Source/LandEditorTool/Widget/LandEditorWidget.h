// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "LandEditorWidget.generated.h"

class ULandEditorDataAssetWidget;
class ULandEditorPieceWidget;

UINTERFACE(MinimalAPI)
class ULandEditorParentWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class ILandEditorParentWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void TriggetInstanceReFresh() = 0;
	virtual void TriggerReFresh() = 0;
	virtual void TriggerSave() = 0;
	virtual void TriggerSaveAndReFresh() = 0;
};

UINTERFACE(MinimalAPI)
class ULandEditorChildWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class ILandEditorChildWidgetInterface
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SVerticalBox> GetWidget() = 0;

	virtual FName GetTabId() = 0;
	virtual FString GetTitleName() = 0;

	virtual bool EnableDraw() = 0;
	virtual void DrawView() = 0;

	virtual void SetParentWidget(ILandEditorParentWidgetInterface* widget)= 0;
	virtual void SetGlobalWidgetInfo(class ULandEditorWidgetInfo* widgetInfo)= 0;
};

UCLASS()
class ULandEditorWidgetInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class ULandDataAsset> DataAssetInstance{nullptr};

	FName DataAssetName;

public:
	class UPieceBaseConfigData* GetPieceConfigDataByLocation(const FPieceLocation location);
	class UPieceBaseConfigData* GetPieceConfigDataById(int id);
	bool GetIsStartPiece(int id);
	bool GetIsFinishPiece(int id);
	bool GetEnableMove(class UPieceBaseConfigData* piece,EPieceDirection direction);
	void DeleteAllPieceByFloor(int floor);
	void CheckAndUpdateMoveDirection(class UPieceBaseConfigData* piece);
};

UCLASS()
class LANDEDITORTOOL_API ULandEditorWidget : public UObject, public ILandEditorParentWidgetInterface
{
	GENERATED_BODY()

	
	ULandEditorWidget(const FObjectInitializer& ObjectInitializer);
	
public:

	TSharedRef<SDockTab> GetWidget();

public:
	static int PieceIdAuto;
	
private:
	TSharedPtr<FTabManager> TabManager;

	UPROPERTY()
	ULandEditorDataAssetWidget* DataAssetWidget;
	
	UPROPERTY()
	ULandEditorPieceWidget* PieceWidget;

	UPROPERTY()
	ULandEditorWidgetInfo* GlobalWidgetInfo;
	
	UPROPERTY()
	TArray<ILandEditorChildWidgetInterface*> ChildWidgets;
	UPROPERTY()
	TArray<ILandEditorChildWidgetInterface*> InstanceChildWidgets;
	
	FTimerHandle SaveTimerHandle;
	FTimerHandle RefreshTimerHandle;
	FTimerHandle RefreshInstanceTimerHandle;
private:

	void CreateChildWidgets();

	void RegisterChildWidgets();
	
	TSharedRef<FTabManager::FLayout> CreateLayout();
	
	FTabSpawnerEntry RegisterTrackedTabSpawner(ILandEditorChildWidgetInterface* childWidget, float maxHeight);

public:
	virtual void TriggetInstanceReFresh() override;
	virtual void TriggerReFresh() override;
	virtual void TriggerSave() override;
	virtual void TriggerSaveAndReFresh() override;
	
private:
	void RefreshInstanceView();
	void ReFreshView();
	void SaveDataAsset();
};
