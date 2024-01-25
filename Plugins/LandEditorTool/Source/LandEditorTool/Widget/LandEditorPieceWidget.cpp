// Fill out your copyright notice in the Description page of Project Settings.


#include "LandEditorPieceWidget.h"

#include "LandEditorCheckerboardWidget.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "Widgets/Input/SSlider.h"

ULandEditorPieceWidget::ULandEditorPieceWidget(const FObjectInitializer& ObjectInitializer)
{
	RootWidget = SNew(SVerticalBox);
	SelectPieceWidget = SNew(SVerticalBox);
	FloorWidget = SNew(SVerticalBox);
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
	RootWidget->AddSlot()
	[
		SNew(SHorizontalBox)

		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			[
				SNew(SVerticalBox)

				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					[
						SelectPieceWidget.ToSharedRef()
					]
				]

				+SVerticalBox::Slot()
				.AutoHeight()
				[
					FloorWidget.ToSharedRef()
				]
			]
		]

		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			CheckerboardWidget->GetWidget()
		]
	];

	DrawSelectPiece();
	DrawFloor();
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
	int pieceId = piece? piece->Id:-1;
	
	SelectPieceWidget->AddSlot()
	.AutoHeight()
	.Padding(4,4)
	[
		SNew(STextBlock)
		.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
		.Text(FText::FromString("Select Piece"))
	];
	
	if(piece == nullptr)
	{
		SelectPieceWidget->AddSlot()
		[
			SNew(SButton)
			.Text(FText::FromString("Create"))
			.OnClicked_Lambda([this,location]()
			{
				CreatePiece(location);
				
				DrawSelectPiece();
				CheckerboardWidget->UpdateLocation(location,GlobalWidgetInfo->GetPieceConfigDataByLocation(location));
				
				ParentWidget->TriggerSave();
				return FReply::Handled();
			})
		];
	}else
	{
		SelectPieceWidget->AddSlot()
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
				.OnCheckStateChanged_Lambda([this,pieceId](ECheckBoxState state)
				{
					if(state == ECheckBoxState::Checked)
					{
						GlobalWidgetInfo->DataAssetInstance->InitialPieceId = pieceId;
						CheckerboardWidget->DrawView();
						DrawSelectPiece();

						ParentWidget->TriggerSave();
					}
				})
			]
		];

		SelectPieceWidget->AddSlot()
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
				.IsChecked(GlobalWidgetInfo->GetIsStartPiece(pieceId))
				.OnCheckStateChanged_Lambda([this,pieceId](ECheckBoxState state)
				{
					if(state == ECheckBoxState::Checked)
					{
						GlobalWidgetInfo->DataAssetInstance->FinishPieceId = pieceId;
						CheckerboardWidget->DrawView();
						DrawSelectPiece();

						ParentWidget->TriggerSave();
					}
				})
			]
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
			DetailsView->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& changeEvent)
			{
				if(changeEvent.HasArchetypeInstanceChanged(GlobalWidgetInfo->DataAssetInstance))
				{
					ParentWidget->TriggerSave();
				}
			});
		}
	}
	
}

void ULandEditorPieceWidget::CreatePiece(FPieceLocation location)
{
	UPieceBaseConfigData* piece = NewObject<UPieceBaseConfigData>();
	piece->Id = ULandEditorWidget::PieceIdAuto;
	piece->BaseInfo = NewObject<UPieceBaseInfo>();
	piece->BaseInfo->Location = location;
	
	ULandEditorWidget::PieceIdAuto+=1;

	GlobalWidgetInfo->DataAssetInstance->Pieces.Add(piece);
}

void ULandEditorPieceWidget::DrawFloor()
{
	FloorWidget->ClearChildren();

	FloorWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Floor:"))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4, 0)
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
				DrawFloor();
				return FReply::Handled();
			})
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FAppStyle::Get().GetBrush("Icons.arrow-up"))
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4, 0)
		[
			SNew(SButton)
			.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
			.OnClicked_Lambda([this]() {
				CheckerboardWidget->CheckerboardWidgetInfo->CurFloor-=1;
				CheckerboardWidget->DrawView();
				DrawFloor();
				return FReply::Handled();
			})
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FAppStyle::Get().GetBrush("Icons.arrow-down"))
			]
		]
	];


	TSharedPtr<SVerticalBox> xSlider = SNew(SVerticalBox);
	xSlider->AddSlot()
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(FText::FromString("X:"))
	];
	xSlider->AddSlot()
	.AutoHeight()
	[
		SNew(SButton)
		.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
		.OnClicked_Lambda([this]() {
				CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X = 0.5f;
				CheckerboardWidget->DrawView();
				DrawFloor();
			return FReply::Handled();
		})
		[
			SNew(SImage)
			.ColorAndOpacity(FSlateColor::UseForeground())
			.Image(FAppStyle::Get().GetBrush("Icons.Redo"))
		]
	];
	xSlider->AddSlot()
	.FillHeight(1.0)
	[
		SNew(SSlider)
	   .Orientation(Orient_Vertical)
	   .Value(CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X)
	   .OnValueChanged_Lambda([this](float value)
	   {
			   CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.X = value;
			   CheckerboardWidget->DrawView();
	   })
	];


	TSharedPtr<SVerticalBox> ySlider = SNew(SVerticalBox);
	ySlider->AddSlot()
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Y:"))
	];
	ySlider->AddSlot()
	.AutoHeight()
	[
		SNew(SButton)
		.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
		.OnClicked_Lambda([this]() {
				CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y = 0.5f;
				CheckerboardWidget->DrawView();
				DrawFloor();
			return FReply::Handled();
		})
		[
			SNew(SImage)
			.ColorAndOpacity(FSlateColor::UseForeground())
			.Image(FAppStyle::Get().GetBrush("Icons.Redo"))
		]
	];
	ySlider->AddSlot()
	.FillHeight(1.0)
	[
		SNew(SSlider)
	   .Orientation(Orient_Vertical)
	   .Value(CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y)
	   .OnValueChanged_Lambda([this](float value)
	   {
			   CheckerboardWidget->CheckerboardWidgetInfo->CurCenterSlider.Y = value;
			   CheckerboardWidget->DrawView();
	   })
	];
	
	FloorWidget->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			[
				xSlider.ToSharedRef()
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			[
				ySlider.ToSharedRef()
			]
		]
	];
	
}