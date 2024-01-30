// Fill out your copyright notice in the Description page of Project Settings.


#include "LandEditorWidget.h"

#include "FileHelpers.h"
#include "LandEditorDataAssetWidget.h"
#include "LandEditorPieceWidget.h"
#include "Widgets/Layout/SScrollBox.h"

int ULandEditorWidget::PieceIdAuto = 0;

ULandEditorWidget::ULandEditorWidget(const FObjectInitializer& ObjectInitializer)
{
	GlobalWidgetInfo = NewObject<ULandEditorWidgetInfo>();
}

TSharedRef<SDockTab> ULandEditorWidget::GetWidget()
{
	CreateChildWidgets();
	
	TSharedRef<SDockTab> contentTab = SNew(SDockTab);
	TabManager = FGlobalTabmanager::Get()->NewTabManager(contentTab);
	
	RegisterChildWidgets();
	
	FVector2D size(1550.f, 770.f);
	
	TSharedRef<FTabManager::FLayout> layout = CreateLayout();
	contentTab->SetContent(
		SNew(SBorder)
		.DesiredSizeScale(size)
		[
			TabManager->RestoreFrom(layout, nullptr).ToSharedRef()
		]
	);

	ReFreshView();
	
	return contentTab;
}

void ULandEditorWidget::CreateChildWidgets()
{
	DataAssetWidget = NewObject<ULandEditorDataAssetWidget>();
	PieceWidget = NewObject<ULandEditorPieceWidget>();

	ChildWidgets.Add(Cast<ILandEditorChildWidgetInterface>(DataAssetWidget));
	ChildWidgets.Add(Cast<ILandEditorChildWidgetInterface>(PieceWidget));

	InstanceChildWidgets.Add(Cast<ILandEditorChildWidgetInterface>(PieceWidget));
	
	ILandEditorParentWidgetInterface* parentWidget = Cast<ILandEditorParentWidgetInterface>(this);
	for(int index = 0;index<ChildWidgets.Num();++index)
	{
		ChildWidgets[index]->SetGlobalWidgetInfo(GlobalWidgetInfo);
		ChildWidgets[index]->SetParentWidget(parentWidget);
	}
}

void ULandEditorWidget::RegisterChildWidgets()
{
	RegisterTrackedTabSpawner(Cast<ILandEditorChildWidgetInterface>(DataAssetWidget),200);
	RegisterTrackedTabSpawner(Cast<ILandEditorChildWidgetInterface>(PieceWidget),1000);
}


TSharedRef<FTabManager::FLayout> ULandEditorWidget::CreateLayout()
{
	const FName tabLayoutName = "LandEditorWidget_Layout";
	return FTabManager::NewLayout(tabLayoutName)
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetHideTabWell(true)
				->AddTab(DataAssetWidget->GetTabId(), ETabState::OpenedTab)
			)
			
			->Split
			(
				FTabManager::NewStack()
				->SetHideTabWell(true)
				->AddTab(PieceWidget->GetTabId(), ETabState::OpenedTab)
			)
		);
}

FTabSpawnerEntry ULandEditorWidget::RegisterTrackedTabSpawner(ILandEditorChildWidgetInterface* childWidget, float maxHeight)
{
	return TabManager->RegisterTabSpawner(childWidget->GetTabId(), FOnSpawnTab::CreateLambda([childWidget,maxHeight](const FSpawnTabArgs& Args) -> TSharedRef<SDockTab>
		{
			TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
				.Label(FText::FromString(childWidget->GetTitleName()))
				.ShouldAutosize(true)
				[
					SNew(SVerticalBox)
						
					+ SVerticalBox::Slot()
					.MaxHeight(maxHeight)
					[
						SNew(SScrollBox)
						.ScrollBarVisibility(EVisibility::Visible)
						.Style(&FCoreStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBox"))
						+ SScrollBox::Slot()
						[
							childWidget->GetWidget()
						]
					]
				];
			return SpawnedTab;
		}))
		.SetDisplayName(FText::FromString(childWidget->GetTitleName()));
}

void ULandEditorWidget::TriggetInstanceReFresh()
{
	if(RefreshInstanceTimerHandle.IsValid())
	{
		return;
	}
	
	FWorldContext worldContext = GEditor->GetEditorWorldContext();
	if (UWorld* world = worldContext.World())
	{
		world->GetTimerManager().SetTimer(RefreshInstanceTimerHandle,[this]()
		{
			RefreshInstanceTimerHandle.Invalidate();
			RefreshInstanceView();
		},0.2f,false);
	}
}

void ULandEditorWidget::RefreshInstanceView()
{
	if(GlobalWidgetInfo == nullptr)
	{
		GlobalWidgetInfo = NewObject<ULandEditorWidgetInfo>();
		GlobalWidgetInfo->DataAssetInstance = nullptr;
		
		for(int index = 0;index<ChildWidgets.Num();++index)
		{
			ChildWidgets[index]->SetGlobalWidgetInfo(GlobalWidgetInfo);
		}
	}
	
	for(int index = 0;index<InstanceChildWidgets.Num();++index)
	{
		InstanceChildWidgets[index]->GetWidget()->ClearChildren();
	}
	
	for(int index = 0;index<InstanceChildWidgets.Num();++index)
	{
		if(InstanceChildWidgets[index]->EnableDraw())
		{
			InstanceChildWidgets[index]->DrawView();
		}
	}
}

void ULandEditorWidget::TriggerReFresh()
{
	if(RefreshTimerHandle.IsValid())
	{
		return;
	}
	
	FWorldContext worldContext = GEditor->GetEditorWorldContext();
	if (UWorld* world = worldContext.World())
	{
		world->GetTimerManager().SetTimer(RefreshTimerHandle,[this]()
		{
			RefreshTimerHandle.Invalidate();
			ReFreshView();
		},0.2f,false);
	}
}

void ULandEditorWidget::ReFreshView()
{
	if(GlobalWidgetInfo == nullptr)
	{
		GlobalWidgetInfo = NewObject<ULandEditorWidgetInfo>();
		GlobalWidgetInfo->DataAssetInstance = nullptr;
		
		for(int index = 0;index<ChildWidgets.Num();++index)
		{
			ChildWidgets[index]->SetGlobalWidgetInfo(GlobalWidgetInfo);
		}
	}

	for(int index = 0;index<ChildWidgets.Num();++index)
	{
		ChildWidgets[index]->GetWidget()->ClearChildren();
	}
	
	for(int index = 0;index<ChildWidgets.Num();++index)
	{
		if(ChildWidgets[index]->EnableDraw())
		{
			ChildWidgets[index]->DrawView();
		}
	}
}

void ULandEditorWidget::TriggerSave()
{
	if(SaveTimerHandle.IsValid())
	{
		return;
	}
	
	FWorldContext worldContext = GEditor->GetEditorWorldContext();
	if (UWorld* world = worldContext.World())
	{
		world->GetTimerManager().SetTimer(SaveTimerHandle,[this]()
		{
			SaveTimerHandle.Invalidate();
			SaveDataAsset();
		},0.1f,false);
	}
}

void ULandEditorWidget::SaveDataAsset()
{
	GlobalWidgetInfo->DataAssetInstance->Modify();

	UPackage* package = GlobalWidgetInfo->DataAssetInstance->GetOutermost();
	FEditorFileUtils::PromptForCheckoutAndSave({ package }, false, false);
}

void ULandEditorWidget::TriggerSaveAndReFresh()
{
	TriggerSave();
	TriggetInstanceReFresh();
}

class UPieceBaseConfigData* ULandEditorWidgetInfo::GetPieceConfigDataByLocation(const FPieceLocation location)
{
	if(!DataAssetInstance)
	{
		return nullptr;
	}

	for(int index = 0;index<DataAssetInstance->Pieces.Num();++index)
	{
		TObjectPtr<UPieceBaseConfigData> piece = DataAssetInstance->Pieces[index];
		if(piece->BaseInfo->Location == location)
		{
			return piece.Get();
		}
	}
	return nullptr;
}

class UPieceBaseConfigData* ULandEditorWidgetInfo::GetPieceConfigDataById(int id)
{
	if(!DataAssetInstance)
	{
		return nullptr;
	}

	for(int index = 0;index<DataAssetInstance->Pieces.Num();++index)
	{
		TObjectPtr<UPieceBaseConfigData> piece = DataAssetInstance->Pieces[index];
		if(piece->Id == id)
		{
			return piece.Get();
		}
	}
	return nullptr;
}

bool ULandEditorWidgetInfo::GetIsStartPiece(int id)
{
	if(!DataAssetInstance)
	{
		return false;
	}

	return DataAssetInstance->InitialPieceId == id;	
}
bool ULandEditorWidgetInfo::GetIsFinishPiece(int id)
{
	if(!DataAssetInstance)
	{
		return false;
	}

	return DataAssetInstance->FinishPieceId == id;	
}

bool ULandEditorWidgetInfo::GetEnableMove(class UPieceBaseConfigData* piece,EPieceDirection direction)
{
	for(int index = 0;index<piece->BaseInfo->EnableDirections.Num();++index)
	{
		if(piece->BaseInfo->EnableDirections[index] == direction)
		{
			return true;
		}
	}

	return false;
}

void ULandEditorWidgetInfo::DeleteAllPieceByFloor(int floor)
{
	int total = DataAssetInstance->Pieces.Num();
	for(int index = 0,id = 0;index<total;++index)
	{
		TObjectPtr<UPieceBaseConfigData> piece = DataAssetInstance->Pieces[id];
		if(piece == nullptr || piece->BaseInfo->Location.Floor == floor)
		{
			DataAssetInstance->Pieces.RemoveAt(id);
			continue;
		}
		id+=1;
	}
}

void ULandEditorWidgetInfo::CheckAndUpdateMoveDirection(class UPieceBaseConfigData* piece)
{
	if(DataAssetInstance->InitialPieceId == piece->Id)
	{
		piece->BaseInfo->EnableDirections.Remove(EPieceDirection::Up);
		piece->BaseInfo->EnableDirections.Remove(EPieceDirection::Down);
	}
	else if(DataAssetInstance->FinishPieceId == piece->Id)
	{
		piece->BaseInfo->EnableDirections.Empty();
	}
	else if(GetEnableMove(piece,EPieceDirection::Up))
	{
		piece->BaseInfo->EnableDirections.Empty();
		piece->BaseInfo->EnableDirections.Add(EPieceDirection::Up);
	}
	else if(GetEnableMove(piece,EPieceDirection::Down))
	{
		piece->BaseInfo->EnableDirections.Empty();
		piece->BaseInfo->EnableDirections.Add(EPieceDirection::Down);
	}
}