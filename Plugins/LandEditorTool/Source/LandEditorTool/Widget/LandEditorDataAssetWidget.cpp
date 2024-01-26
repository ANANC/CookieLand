// Fill out your copyright notice in the Description page of Project Settings.


#include "LandEditorDataAssetWidget.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "LandEditorToolStyle.h"
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
	if(!GlobalWidgetInfo->DataAssetInstance.Get())
	{
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
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
					]
				]
			];
	}
	else
	{
		RootWidget->AddSlot()
		.AutoHeight()
		[

			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("DataAsset:"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4,0)
			[
				SNew(STextBlock)
				.Text(FText::FromName(GlobalWidgetInfo->DataAssetName))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4,0)
			[
				SNew(SButton)
				.OnClicked_Lambda([this]() {
					GlobalWidgetInfo->DataAssetInstance = nullptr;
					ParentWidget->TriggerReFresh();
					return FReply::Handled();
				})
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(FLandEditorToolStyle::Get().GetBrush( "Icons.redo"))
				]
			]
		];
	}
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
			CheckLandData();

			ParentWidget->TriggerReFresh();
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
	int total = GlobalWidgetInfo->DataAssetInstance->Pieces.Num();
	for(int index = 0,id = 0;index<total;++index)
	{
		TObjectPtr<UPieceBaseConfigData> piece = GlobalWidgetInfo->DataAssetInstance->Pieces[id];
		if(piece == nullptr)
		{
			GlobalWidgetInfo->DataAssetInstance->Pieces.RemoveAt(id);
			continue;
		}
		id+=1;
		if(piece->Id > ULandEditorWidget::PieceIdAuto)
		{
			ULandEditorWidget::PieceIdAuto = piece->Id;
		}
	}
	
	ULandEditorWidget::PieceIdAuto += 1;

	if(total!=GlobalWidgetInfo->DataAssetInstance->Pieces.Num())
	{
		ParentWidget->TriggerSave();
	}
}


void ULandEditorDataAssetWidget::CheckLandData()
{
	if(!GlobalWidgetInfo->GetPieceConfigDataById(GlobalWidgetInfo->DataAssetInstance->InitialPieceId))
	{
		GlobalWidgetInfo->DataAssetInstance->InitialPieceId = -1;
		ParentWidget->TriggerSave();
	}
	if(!GlobalWidgetInfo->GetPieceConfigDataById(GlobalWidgetInfo->DataAssetInstance->FinishPieceId))
	{
		GlobalWidgetInfo->DataAssetInstance->FinishPieceId = -1;
		ParentWidget->TriggerSave();
	}
}