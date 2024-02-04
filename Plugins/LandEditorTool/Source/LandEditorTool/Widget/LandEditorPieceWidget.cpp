// Fill out your copyright notice in the Description page of Project Settings.


#include "LandEditorPieceWidget.h"

#include "LandEditorCheckerboardWidget.h"
#include "LandEditorToolStyle.h"
#include "CookieLand/Piece/BasePieceActor.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SScrollBox.h"

ULandEditorPieceWidget::ULandEditorPieceWidget(const FObjectInitializer& ObjectInitializer)
{
	RootWidget = SNew(SVerticalBox);
	SelectPieceWidget = SNew(SVerticalBox);
	BottomWidget = SNew(SVerticalBox);
	Bottom_FloorWidget = SNew(SVerticalBox);
	Bottom_SliderWidget = SNew(SVerticalBox);
	CheckerboardWidget = NewObject<ULandEditorCheckerboardWidget>();
	CheckerboardWidget->LandEditorCheckerboardWidgetSelectChangeEvent.BindUObject(this,&ULandEditorPieceWidget::LandEditorCheckerboardWidgetSelectChangeEventCallback);
}

void ULandEditorPieceWidget::SetGlobalWidgetInfo(class ULandEditorWidgetInfo* widgetInfo)
{
	GlobalWidgetInfo = widgetInfo;
	CheckerboardWidget->SetGlobalWidgetInfo(GlobalWidgetInfo);
}


TSharedRef<SVerticalBox> ULandEditorPieceWidget::GetWidget()
{
	return RootWidget.ToSharedRef();
}

bool ULandEditorPieceWidget::EnableDraw()
{
	return GlobalWidgetInfo!=nullptr && GlobalWidgetInfo->DataAssetInstance!=nullptr;
}

void ULandEditorPieceWidget::DrawView()
{
	UPieceBaseConfigData* piece = GlobalWidgetInfo->GetPieceConfigDataById(GlobalWidgetInfo->DataAssetInstance->InitialPieceId);
	if(piece)
	{
		CheckerboardWidget->CheckerboardWidgetInfo->CurFloor = piece->BaseInfo->Location.Floor;
		CheckerboardWidget->CheckerboardWidgetInfo->SelectLocation = FVector(piece->BaseInfo->Location.X,piece->BaseInfo->Location.Y,piece->BaseInfo->Location.Floor);
	}

	RootWidget->AddSlot()
	[
		SNew(SHorizontalBox)
		
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			CheckerboardWidget->GetWidget()
		]
		
		+SHorizontalBox::Slot()
        .AutoWidth()
		.Padding(10,0)
        [
        	SNew(SBorder)
        	[
        		BottomWidget.ToSharedRef()
        	]
        ]
        
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4,0)
		[
			SNew(SBorder)
			[
				SNew(SVerticalBox)

				+SVerticalBox::Slot()
				.MaxHeight(700)
				[
					SNew(SScrollBox)
					.ScrollBarVisibility(EVisibility::Visible)
					+ SScrollBox::Slot()
					[
						SelectPieceWidget.ToSharedRef()
					]
				]
			]
		]
		
	];

	DrawSelectPiece();
	DrawBottom();
	CheckerboardWidget->DrawView();
}

void ULandEditorPieceWidget::LandEditorCheckerboardWidgetSelectChangeEventCallback()
{
	DrawSelectPiece();
}

void ULandEditorPieceWidget::DrawSelectPiece()
{
	SelectPieceWidget->ClearChildren();

	FPieceLocation location = CheckerboardWidget->ChangeToGameLocation(CheckerboardWidget->CheckerboardWidgetInfo->SelectLocation);
	class UPieceBaseConfigData* piece = GlobalWidgetInfo->GetPieceConfigDataByLocation(location);
	
	SelectPieceWidget->AddSlot()
	.AutoHeight()
	.Padding(4,4)
	[
		SNew(STextBlock)
		.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
		.Text(FText::FromString("Select Piece"))
	];
	
	FString locationStr = "x:"+FString::FromInt(location.X)+" y:"+FString::FromInt(location.Y);
	SelectPieceWidget->AddSlot()
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(FText::FromString(locationStr))
	];


	SelectPieceWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Is Auto Create Empty Piece:"))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
			.IsChecked(IsAutoCreate)
			.OnCheckStateChanged_Lambda([this](ECheckBoxState state)
			{
				IsAutoCreate = state == ECheckBoxState::Checked? true:false;
			})
		]
	];
	
	FVector seleceLocation = CheckerboardWidget->CheckerboardWidgetInfo->SelectLocation;
	int curFloor = CheckerboardWidget->CheckerboardWidgetInfo->CurFloor;
	
	for(int y = -1;y<2;++y)
	{
		TSharedPtr<SHorizontalBox> horizontalBox = SNew(SHorizontalBox);
		for(int x = -1;x<2;++x)
		{
			FVector curLogicLocation = FVector(seleceLocation.X+x,seleceLocation.Y+y,curFloor);

			bool isMyself = curLogicLocation == seleceLocation;
			bool hasPiece = GlobalWidgetInfo->GetPieceConfigDataByLocation(CheckerboardWidget->ChangeToGameLocation(curLogicLocation)) != nullptr;
			
			horizontalBox->AddSlot()
			.AutoWidth()
			.Padding(4,4)
			[
				SNew(SButton)
				.Text(FText::FromString(isMyself? "   ":hasPiece?" * ":" + "))
				.OnClicked_Lambda([this,isMyself,curLogicLocation,hasPiece]() {
					FPieceLocation gameLocation = CheckerboardWidget->ChangeToGameLocation(curLogicLocation);
					if(!isMyself)
					{
						if(!hasPiece && IsAutoCreate)
						{
							CreatePiece(gameLocation);
						}
						CheckerboardWidget->SelectLocation(gameLocation);
					}
					return FReply::Handled();
				})
			];
		}
		SelectPieceWidget->AddSlot()
		.AutoHeight()
		[
			horizontalBox.ToSharedRef()
		];
	}

	if(piece == nullptr)
	{
		DrawEmptyPiece();
	}else
	{
		DrawValidPiece(piece);
	}

}

void ULandEditorPieceWidget::DrawEmptyPiece()
{
	FVector seleceLocation = CheckerboardWidget->CheckerboardWidgetInfo->SelectLocation;
	
	SelectPieceWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SButton)
		.Text(FText::FromString("Create"))
		.OnClicked_Lambda([this,seleceLocation]()
		{
			FPieceLocation location = CheckerboardWidget->ChangeToGameLocation(seleceLocation);
			CreatePiece(location);
			
			DrawSelectPiece();
			CheckerboardWidget->UpdateLocation(location);
			
			ParentWidget->TriggerSave();
			return FReply::Handled();
		})
	];
}

void ULandEditorPieceWidget::DrawValidPiece(class UPieceBaseConfigData* piece)
{
	FVector seleceLocation = CheckerboardWidget->CheckerboardWidgetInfo->SelectLocation;
	int pieceId = piece->Id;
	int curFloor = CheckerboardWidget->CheckerboardWidgetInfo->CurFloor;
	
	
	SelectPieceWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Is Initial Location:"))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
			.IsChecked(GlobalWidgetInfo->GetIsStartPiece(pieceId))
			.OnCheckStateChanged_Lambda([this,pieceId,seleceLocation](ECheckBoxState state)
			{
				FPieceLocation location = CheckerboardWidget->ChangeToGameLocation(seleceLocation);
				
				int oldId=GlobalWidgetInfo->DataAssetInstance->InitialPieceId;
				if(state == ECheckBoxState::Checked)
				{
					GlobalWidgetInfo->DataAssetInstance->InitialPieceId = pieceId;
				}
				else
				{
					GlobalWidgetInfo->DataAssetInstance->InitialPieceId = -1;
				}
				CheckerboardWidget->UpdateLocation(oldId);
				CheckerboardWidget->UpdateLocation(location);

				UPieceBaseConfigData* piece = GlobalWidgetInfo->GetPieceConfigDataById(GlobalWidgetInfo->DataAssetInstance->InitialPieceId);
				if(piece)
				{
					GlobalWidgetInfo->CheckAndUpdateMoveDirection(piece);
				}
				
				ParentWidget->TriggerSave();
			})
		]
	];

	SelectPieceWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Is Finish Location:"))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
			.IsChecked(GlobalWidgetInfo->GetIsFinishPiece(pieceId))
			.OnCheckStateChanged_Lambda([this,pieceId,seleceLocation](ECheckBoxState state)
			{
				FPieceLocation location = CheckerboardWidget->ChangeToGameLocation(seleceLocation);
				
				int oldId=GlobalWidgetInfo->DataAssetInstance->FinishPieceId;
				if(state == ECheckBoxState::Checked)
				{
					GlobalWidgetInfo->DataAssetInstance->FinishPieceId = pieceId;
				}
				else
				{
					GlobalWidgetInfo->DataAssetInstance->FinishPieceId = -1;
				}
				CheckerboardWidget->UpdateLocation(oldId);
				CheckerboardWidget->UpdateLocation(location);

				UPieceBaseConfigData* piece = GlobalWidgetInfo->GetPieceConfigDataById(GlobalWidgetInfo->DataAssetInstance->FinishPieceId);
				if(piece)
				{
					GlobalWidgetInfo->CheckAndUpdateMoveDirection(piece);
				}
				
				ParentWidget->TriggerSave();
			})
		]
	];

	SelectPieceWidget->AddSlot()
	.Padding(0,6)
	.AutoHeight()
	[
		SNew(SButton)
		.Text(FText::FromString("Delete"))
		.OnClicked_Lambda([this,seleceLocation]()
		{
			FPieceLocation location = CheckerboardWidget->ChangeToGameLocation(seleceLocation);
			
			DeletePiece(location);
			DrawSelectPiece();
			CheckerboardWidget->UpdateLocation(location);
			return FReply::Handled();
		})
	];
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bShowObjectLabel = false;
	TSharedPtr< class IDetailsView > DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	piece->SetFlags(RF_Transactional);
	if (DetailsView.IsValid())
	{
		// Make sure details window is pointing to our object
		DetailsView->SetObject(piece);
		DetailsView->OnFinishedChangingProperties().AddLambda([this,seleceLocation](const FPropertyChangedEvent& changeEvent)
		{
			if(changeEvent.HasArchetypeInstanceChanged(GlobalWidgetInfo->DataAssetInstance))
			{
				FPieceLocation location = CheckerboardWidget->ChangeToGameLocation(seleceLocation);
				
				UPieceBaseConfigData* piece = GlobalWidgetInfo->GetPieceConfigDataByLocation(location);
				if(piece)
				{
					GlobalWidgetInfo->CheckAndUpdateMoveDirection(piece);
				}
				
				ParentWidget->TriggerSave();
				CheckerboardWidget->UpdateLocation(location);
			}
		});
	}

	SelectPieceWidget->AddSlot()
	.AutoHeight()
	[
		DetailsView.ToSharedRef()
	];
	
}

void ULandEditorPieceWidget::CreatePiece(FPieceLocation location)
{
	if(GlobalWidgetInfo->GetPieceConfigDataByLocation(location))
	{
		return;
	}
	
	FProperty* prop = FindFieldChecked<FProperty>(ULandDataAsset::StaticClass(), FName("Pieces"));
	if (const FArrayProperty* arrayProp = CastField<FArrayProperty>(prop))
	{
		FObjectProperty* actionProp = CastField<FObjectProperty>(arrayProp->Inner);

		UPieceBaseConfigData* piece = NewObject< UPieceBaseConfigData>(GlobalWidgetInfo->DataAssetInstance.Get(), UPieceBaseConfigData::StaticClass());

		FScriptArrayHelper_InContainer helper(arrayProp, GlobalWidgetInfo->DataAssetInstance.Get());
		int32 newIndex = helper.AddValue();
		actionProp->CopyCompleteValue(helper.GetRawPtr(newIndex), &piece);

		piece->Id = ULandEditorWidget::PieceIdAuto;
		piece->BaseInfo = NewObject<UPieceBaseInfo>(piece,UPieceBaseInfo::StaticClass());
		piece->BaseInfo->Location = location;

		ULandEditorWidget::PieceIdAuto+=1;
	
		ParentWidget->TriggerSave();

		DrawFloor();
	}
}

void ULandEditorPieceWidget::DeletePiece(FPieceLocation location)
{
	UPieceBaseConfigData* piece = GlobalWidgetInfo->GetPieceConfigDataByLocation(location);
	if(piece)
	{
		GlobalWidgetInfo->DataAssetInstance->Pieces.Remove(piece);

		ParentWidget->TriggerSave();

		DrawFloor();
	}
}

void ULandEditorPieceWidget::DrawBottom()
{
	BottomWidget->ClearChildren();

	BottomWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SBox)
		[
			Bottom_FloorWidget.ToSharedRef()
		]
	];
	BottomWidget->AddSlot()
	.AutoHeight()
	.Padding(0,10)
	[
		SNew(SBox)
		[
			Bottom_SliderWidget.ToSharedRef()
		]
	];

	DrawFloor();
	DrawSlider();
}

void ULandEditorPieceWidget::DrawFloor()
{
	Bottom_FloorWidget->ClearChildren();

	Bottom_FloorWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0,5)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Floor:"))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4, 5)
		[
			SNew(STextBlock)
			.Text(FText::FromString(FString::FromInt(CheckerboardWidget->CheckerboardWidgetInfo->CurFloor)))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(8, 0)
		[
			SNew(SButton)
			.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
			.OnClicked_Lambda([this]() {
				CheckerboardWidget->CheckerboardWidgetInfo->CurFloor+=1;
				CheckerboardWidget->DrawView();
				DrawBottom();
				return FReply::Handled();
			})
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FLandEditorToolStyle::Get().GetBrush("Icons.arrow-up"))
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
			.OnClicked_Lambda([this]() {
				CheckerboardWidget->CheckerboardWidgetInfo->CurFloor-=1;
				CheckerboardWidget->DrawView();
				DrawBottom();
				return FReply::Handled();
			})
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FLandEditorToolStyle::Get().GetBrush("Icons.arrow-down"))
			]
		]
	];

	TArray<int> floorIndexs;
	for(int index = 0;index<GlobalWidgetInfo->DataAssetInstance->Pieces.Num();++index)
	{
		floorIndexs.AddUnique(GlobalWidgetInfo->DataAssetInstance->Pieces[index]->BaseInfo->Location.Floor);
	}
	floorIndexs.Sort([](int a,int b){return a>b;});

	if(floorIndexs.Num()>0)
	{
		Bottom_FloorWidget->AddSlot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Valid Floors:"))
		];
	}
	for(int index = 0;index<floorIndexs.Num();++index)
	{
		int curFloor = floorIndexs[index];
		Bottom_FloorWidget->AddSlot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString(FString::FromInt(curFloor)))
			.OnClicked_Lambda([this,curFloor]() {
				if(curFloor != CheckerboardWidget->CheckerboardWidgetInfo->CurFloor)
				{
					CheckerboardWidget->CheckerboardWidgetInfo->CurFloor=curFloor;
					CheckerboardWidget->DrawView();
					DrawBottom();
				}
				return FReply::Handled();
			})
		];
	}
	
	Bottom_FloorWidget->AddSlot()
	.AutoHeight()
	.Padding(0,10)
	[
		SNew(SButton)
		.Text(FText::FromString("Delete Floor Pieces"))
		.OnClicked_Lambda([this]() {
			if(GlobalWidgetInfo->DataAssetInstance)
			{
				GlobalWidgetInfo->DeleteAllPieceByFloor(CheckerboardWidget->CheckerboardWidgetInfo->CurFloor);
				ParentWidget->TriggerSave();

				CheckerboardWidget->DrawView();
				DrawSelectPiece();
			}
			return FReply::Handled();
		})
	];

	Bottom_FloorWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SButton)
		.Text(FText::FromString("Delete All Pieces"))
		.OnClicked_Lambda([this]() {
			if(GlobalWidgetInfo->DataAssetInstance)
			{
				GlobalWidgetInfo->DataAssetInstance->Pieces.Empty();
				ParentWidget->TriggerSave();

				CheckerboardWidget->DrawView();
				DrawSelectPiece();
			}
			return FReply::Handled();
		})
	];
}

void ULandEditorPieceWidget::DrawSlider()
{
	Bottom_SliderWidget->ClearChildren();
	
	Bottom_SliderWidget->AddSlot()
	.AutoHeight()
	[

		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("X"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
				.OnClicked_Lambda([this]() {
						CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X = 0.5f;
						CheckerboardWidget->DrawView();
					return FReply::Handled();
				})
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(FLandEditorToolStyle::Get().GetBrush("Icons.redo"))
				]
			]
		]
	
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SNumericEntryBox<float>)
			.MinValue(0.0f)
			.MaxValue(1.0f)
			.MinSliderValue(TAttribute< TOptional<float> >(0.0f))
			.MaxSliderValue(TAttribute< TOptional<float> >(1.0f))
			.Delta(0.1f)
			.Value(CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X)
			.AllowSpin(true)
			.OnValueChanged_Lambda( [this] (float value) {
				CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X = value;
				CheckerboardWidget->DrawView(); } )
			.OnValueCommitted_Lambda( [this] (float value, ETextCommit::Type CommitInfo) {
				CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X = value;
				CheckerboardWidget->DrawView(); } )
			.Value_Lambda( [this] { return TOptional<float>(CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X); } )
		]
	];
	
	Bottom_SliderWidget->AddSlot()
	.AutoHeight()
	[

		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Y"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
				.OnClicked_Lambda([this]() {
						CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y = 0.5f;
						CheckerboardWidget->DrawView();
					return FReply::Handled();
				})
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(FLandEditorToolStyle::Get().GetBrush("Icons.redo"))
				]
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SNumericEntryBox<float>)
			.MinValue(0.0f)
			.MaxValue(1.0f)
			.MinSliderValue(TAttribute< TOptional<float> >(0.0f))
			.MaxSliderValue(TAttribute< TOptional<float> >(1.0f))
			.Delta(0.1f)
			.Value(CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y)
			.AllowSpin(true)
			.OnValueChanged_Lambda( [this] (float value) {
				CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y = value;
				CheckerboardWidget->DrawView(); } )
			.OnValueCommitted_Lambda( [this] (float value, ETextCommit::Type CommitInfo) {
				CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y = value;
				CheckerboardWidget->DrawView(); } )
			.Value_Lambda( [this] { return TOptional<float>(CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y); } )

		]
			
	];
}