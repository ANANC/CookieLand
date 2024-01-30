// Fill out your copyright notice in the Description page of Project Settings.


#include "LandEditorCheckerboardWidget.h"

#include "LandEditorToolStyle.h"
#include "CookieLand/Piece/PieceTypes.h"
#include "Widgets/SBoxPanel.h"


ULandEditorCheckerboardWidget::ULandEditorCheckerboardWidget(const FObjectInitializer& ObjectInitializer)
{
	RootWidget = SNew(SVerticalBox);
	CheckerboardWidgetInfo = NewObject<ULandEditorCheckerboardWidgetInfo>();
}

TSharedRef<SVerticalBox> ULandEditorCheckerboardWidget::GetWidget()
{
	return RootWidget.ToSharedRef();
}

bool ULandEditorCheckerboardWidget::EnableDraw()
{
	return GlobalWidgetInfo!=nullptr && GlobalWidgetInfo->DataAssetInstance!=nullptr;
}

void ULandEditorCheckerboardWidget::DrawView()
{
	RootWidget->ClearChildren();
	PieceBoarders.Reset();
	
	int radius_x = (int)(CheckerboardWidgetInfo->LandSize.X * 0.5f);
	int radius_y = (int)(CheckerboardWidgetInfo->LandSize.Y * 0.5f);

	int startLocation_x = (int)(CheckerboardWidgetInfo->LandSize.X * CheckerboardWidgetInfo->CurCenterSlider.X) - radius_x;
	int startLocation_y = (int)(CheckerboardWidgetInfo->LandSize.Y * CheckerboardWidgetInfo->CurCenterSlider.Y) - radius_x;
	startLocation_x = int(FMath::Max(-radius_x,startLocation_x));
	startLocation_y = int(FMath::Max(-radius_y,startLocation_y));

	for(int y = -1;y<CheckerboardWidgetInfo->DrawSize.Y;++y)
	{
		TSharedPtr<SHorizontalBox> horizontalBox = SNew(SHorizontalBox);
		
		RootWidget->AddSlot()
		.AutoHeight()
		[
			horizontalBox.ToSharedRef()
		];

		for(int x = -1;x<CheckerboardWidgetInfo->DrawSize.X;++x)
		{
			int curX = startLocation_x + x;
			int curY = startLocation_y + y;

			//头数
			if(y == -1 && x !=-1)
			{
				if(x == 0)
				{
					horizontalBox->AddSlot()
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(size_x)
						.HeightOverride(size_y)
						.VAlign(VAlign_Bottom)
						[
							SNew(STextBlock)
							.Text(FText::FromString("              "))
						]
					];
				}
				horizontalBox->AddSlot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(size_x+8)
					.HeightOverride(size_y)
					.VAlign(VAlign_Bottom)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::FromInt(curX)))
					]
				];

			}

			//旁数
			if(y != -1 && x == -1)
			{
				horizontalBox->AddSlot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(size_x)
					.HeightOverride(size_y)
					.HAlign(HAlign_Right)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::FromInt(curY)))
					]
				];
			}

			//内容
			if(y!=-1 && x!=-1)
			{
				FPieceLocation location(curX,curY,CheckerboardWidgetInfo->CurFloor);
				
				UPieceBaseConfigData* pieceConfigData = GlobalWidgetInfo->GetPieceConfigDataByLocation(location);

				TSharedPtr<SBorder> border = SNew(SBorder);
				PieceBoarders.Add(ChangeToLogicLocation(location),border);
				
				horizontalBox->AddSlot()
				.AutoWidth()
				[
					border.ToSharedRef()
				];

				UpdateLocation(location);
			}

		}
	}

	CheckerboardWidgetInfo->SelectLocation.Z = CheckerboardWidgetInfo->CurFloor;
	SelectLocation(ChangeToGameLocation(CheckerboardWidgetInfo->SelectLocation));
}

void ULandEditorCheckerboardWidget::UpdateLocation(int id)
{
	class UPieceBaseConfigData* pieceConfigData = GlobalWidgetInfo->GetPieceConfigDataById(id);
	if(pieceConfigData)
	{
		UpdateLocation(pieceConfigData->BaseInfo->Location);
	}
}

void ULandEditorCheckerboardWidget::UpdateLocation(FPieceLocation location)
{
	FVector logicLocation = ChangeToLogicLocation(location);
	TSharedPtr<SBorder>* borderPtr = PieceBoarders.Find(logicLocation);
	if(!borderPtr)
	{
		return;
	}
	
	class UPieceBaseConfigData* pieceConfigData = GlobalWidgetInfo?GlobalWidgetInfo->GetPieceConfigDataByLocation(location):nullptr;
	bool isSelect = CheckerboardWidgetInfo->SelectLocation.X == logicLocation.X && CheckerboardWidgetInfo->SelectLocation.Y == logicLocation.Y;

	FSlateColor selectColor = FAppStyle::Get().GetSlateColor("Colors.AccentFolder");
	FSlateColor pieceColor = pieceConfigData? FAppStyle::Get().GetSlateColor("Colors.SelectInactive"):FAppStyle::Get().GetSlateColor("Colors.Header");
	
	TSharedPtr<SBorder> border = *borderPtr;
	
	TSharedPtr<SBox> box = SNew(SBox)
	.WidthOverride(size_x)
	.HeightOverride(size_y)
	[
		SNew(SOverlay)

		+SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderImage( isSelect ?new FSlateRoundedBoxBrush(pieceColor, 6.0f,selectColor,3.f):
				new FSlateRoundedBoxBrush(pieceColor, 6.0f))
		]

		+SOverlay::Slot()
		[
			SNew(SButton)
			.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
			.OnClicked_Lambda([this,logicLocation]() {
				ButtonClickCallback(ChangeToGameLocation(logicLocation));
				return FReply::Handled();
			})
			[
				GetButtonBoxContent(pieceConfigData).ToSharedRef()
			]
		]
	];

	border->SetContent(
		SNew(SBorder)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			box.ToSharedRef()
		]
	);
}

TSharedPtr<SHorizontalBox> ULandEditorCheckerboardWidget::GetButtonBoxContent(class UPieceBaseConfigData* pieceConfigData)
{
	TSharedPtr<SHorizontalBox> HBox = SNew(SHorizontalBox);

	if(pieceConfigData)
	{
		FName iconName;
		if(GlobalWidgetInfo->GetIsFinishPiece(pieceConfigData->Id))
		{
			iconName = "Icons.advanced";
		}
		else if(GlobalWidgetInfo->GetIsStartPiece(pieceConfigData->Id))
		{
			iconName = "Icons.animation";
		}
		else if(GlobalWidgetInfo->GetEnableMove(pieceConfigData,EPieceDirection::Up))
		{
			iconName = "Icons.arrow-up";
		}
		else if(GlobalWidgetInfo->GetEnableMove(pieceConfigData,EPieceDirection::Down))
		{
			iconName = "Icons.arrow-down";
		}

		if(!iconName.IsNone())
		{
			HBox->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FLandEditorToolStyle::Get().GetBrush(iconName))
			];
		}

		if(!pieceConfigData->BaseInfo->Distance.IsUnLimit)
		{
			HBox->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(pieceConfigData->BaseInfo->Distance.LimitDistance)))
			];
		}
	}
	
	return HBox;
}

void ULandEditorCheckerboardWidget::ButtonClickCallback(FPieceLocation location)
{
	FPieceLocation oldSelect = ChangeToGameLocation(CheckerboardWidgetInfo->SelectLocation);
	CheckerboardWidgetInfo->SelectLocation = ChangeToLogicLocation(location);

	UpdateLocation(oldSelect);
	UpdateLocation(location);

	LandEditorCheckerboardWidgetSelectChangeEvent.Execute();
}

void ULandEditorCheckerboardWidget::SelectLocation(FPieceLocation location)
{
	ButtonClickCallback(location);
}

FVector ULandEditorCheckerboardWidget::ChangeToLogicLocation(FPieceLocation location)
{
	FVector logicLocation = FVector(location.X,location.Y,location.Floor);
	return logicLocation;
}

FPieceLocation ULandEditorCheckerboardWidget::ChangeToGameLocation(FVector location)
{
	FPieceLocation gameLocation(location);
	return gameLocation;
}