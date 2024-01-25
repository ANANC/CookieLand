// Fill out your copyright notice in the Description page of Project Settings.


#include "LandEditorDataAssetWidget.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "CookieLand/Piece/PieceTypes.h"

ULandEditorDataAssetWidget::ULandEditorDataAssetWidget(const FObjectInitializer& ObjectInitializer)
{
	RootWidget = SNew(SVerticalBox);
}

TSharedRef<SVerticalBox> ULandEditorDataAssetWidget::GetWidget()
{
	return RootWidget.ToSharedRef();
}

bool ULandEditorDataAssetWidget::EnableDraw()
{
	return GlobalWidgetInfo!=nullptr;
}

void ULandEditorDataAssetWidget::DrawView()
{
	FLinearColor backgroundColor = GlobalWidgetInfo->DataAssetInstance.Get() ? FLinearColor::Green : FLinearColor::Red;

	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.Filter.ClassPaths.Add(ULandDataAsset::StaticClass()->GetClassPathName());
	AssetPickerConfig.Filter.bRecursiveClasses = true;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::Tile;
	AssetPickerConfig.bAllowDragging = false;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateLambda([this]( const FAssetData& selectDA)
	{
		ReLoadDataAsset(*selectDA.ObjectPath.ToString());
	});
	
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	RootWidget->AddSlot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(FMargin(4.0f))
			.BorderBackgroundColor(backgroundColor)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				]
			]
		];
}

void ULandEditorDataAssetWidget::ReLoadDataAsset(const TCHAR* objectPath)
{
	if(GlobalWidgetInfo == nullptr)
	{
		return;
	}

	ULandDataAsset* newDA = LoadObject<ULandDataAsset>(nullptr, objectPath);
	
	FName oldDataAssetName = GlobalWidgetInfo->DataAssetName;
	if(GlobalWidgetInfo->DataAssetInstance.Get())
	{
		GlobalWidgetInfo->DataAssetInstance->RemoveFromRoot();
		GlobalWidgetInfo->DataAssetInstance = nullptr;
	}
	
	if(newDA)
	{
		GlobalWidgetInfo->DataAssetInstance = newDA;
	}
	
	if (GlobalWidgetInfo->DataAssetInstance.Get())
	{
		if (oldDataAssetName != GlobalWidgetInfo->DataAssetInstance->GetFName())
		{
			GlobalWidgetInfo->DataAssetName = GlobalWidgetInfo->DataAssetInstance->GetFName();

			//update GlobalWidgetInfo otherInfo
			InitAutoPieceId();

			ParentWidget->TriggetInstanceReFresh();
		}
	}
	else
	{
		GlobalWidgetInfo->DataAssetName = "";
	}

}

void ULandEditorDataAssetWidget::InitAutoPieceId()
{
	ULandEditorWidget::PieceIdAuto = 0;
	for(int index = 0;index<GlobalWidgetInfo->DataAssetInstance->Pieces.Num();++index)
	{
		UPieceBaseConfigData* piece = GlobalWidgetInfo->DataAssetInstance->Pieces[index];
		if(piece->Id > ULandEditorWidget::PieceIdAuto)
		{
			ULandEditorWidget::PieceIdAuto = piece->Id;
		}
	}
	
	ULandEditorWidget::PieceIdAuto += 1;
}