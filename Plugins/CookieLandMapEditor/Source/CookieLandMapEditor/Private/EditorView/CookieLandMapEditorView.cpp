// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorView/CookieLandMapEditorView.h"
#include "CookieLand/Map/Public/CookieLandMapBuildActor.h"
#include "CookieLand/Map/Public/CookieLandPiece.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"
#include "CookieLand/Map/Public/CookieLandMapBuilder.h"
#include "CookieLand/Map/Public/CookieLandMapActorGather.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "PropertyCustomizationHelpers.h"
#include "FileHelpers.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
#endif // __clang__

#define LOCTEXT_NAMESPACE "CookieLandMapEditorView"

void UCookieLandMapEditorView::Init()
{
	MapEditorContentTyle = ECookieLandMapEditorContentTyle::MapBuildView;
	Checkerboard = NewObject<UCookieLandMapEditor_Checkerboard>();
	SelectMapCube = NewObject<UCookieLandMapEditor_SelectMapCube>();

	Checkerboard->SelectColor = FAppStyle::Get().GetSlateColor("Colors.AccentFolder");
	Checkerboard->BgHasDataColor = FAppStyle::Get().GetSlateColor("Colors.SelectInactive");
	Checkerboard->BgEmptyColor = FAppStyle::Get().GetSlateColor("Colors.Header");

#if WITH_EDITOR
	MapBuildActor = ACookieLandMapBuildActor::MapBuildActorInstance;
	ACookieLandMapBuildActor::MapBuildActorInstance = nullptr;
#endif
}

TSharedRef<SDockTab> UCookieLandMapEditorView::Draw()
{
	ContentViewVerticalBox = SNew(SVerticalBox);

	TSharedPtr<SDockTab> DockTab = SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SBox)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				Draw_TitleButtons().ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				ContentViewVerticalBox.ToSharedRef()
			]
		]
	];

	DrawUpdateContentViewVerticalBox();

	return DockTab.ToSharedRef();
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_TitleButtons()
{
	TSharedPtr<SVerticalBox> TitleVerticalBox = SNew(SVerticalBox);

	TitleVerticalBox->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("MpaBuildViewButtonLabel", "MpaBuildView"))
			.OnClicked_Lambda([this]() -> FReply {
					this->MapEditorContentTyle = ECookieLandMapEditorContentTyle::MapBuildView;
					DrawUpdateContentViewVerticalBox();
					return FReply::Handled();
				})
		]
	];

	return TitleVerticalBox;
}

void UCookieLandMapEditorView::DrawUpdateContentViewVerticalBox()
{
	ContentViewVerticalBox->ClearChildren();

	switch (MapEditorContentTyle)
	{
	case ECookieLandMapEditorContentTyle::MapBuildView:
		ContentViewVerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(FMargin(4.0f))
			[
				Draw_MpaBuildView().ToSharedRef()
			]
		];
		break;
	default:
		break;
	}
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_MpaBuildView()
{
	TSharedPtr<SVerticalBox> MpaBuildVerticalBox = SNew(SVerticalBox);
	SelectMapCube->SelectContextVerticalBox = SNew(SVerticalBox);

	MpaBuildVerticalBox->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.Padding(0,0,12.f,0)
		.AutoWidth()
		[
			Draw_Checkerboard().ToSharedRef()
		]

		+ SHorizontalBox::Slot()
		.Padding(0, 0, 12.f, 0)
		.AutoWidth()
		[
			SNew(SBorder)
			.Padding(FMargin(4.0f))
			[
				Draw_ViewControlContext().ToSharedRef()
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			.Padding(FMargin(4.0f))
			[
				SelectMapCube->SelectContextVerticalBox.ToSharedRef()
			]
		]

	];

	DrawUpdateSelectContext();

	return MpaBuildVerticalBox;

}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_Checkerboard()
{
	TSharedPtr<SVerticalBox> CheckerboardVerticalBox = SNew(SVerticalBox);

	int radius_x = (int)(Checkerboard->LandSize.X * 0.5f);
	int radius_y = (int)(Checkerboard->LandSize.Y * 0.5f);

	int startLocation_x = (int)(Checkerboard->LandSize.X * Checkerboard->CurCenterSlider.X) - radius_x;
	int startLocation_y = (int)(Checkerboard->LandSize.Y * Checkerboard->CurCenterSlider.Y) - radius_x;
	startLocation_x = int(FMath::Max(-radius_x, startLocation_x));
	startLocation_y = int(FMath::Max(-radius_y, startLocation_y));

	Checkerboard->Location2Border.Empty();

	for (int y = -1; y < Checkerboard->DrawSize.Y; ++y)
	{
		TSharedPtr<SHorizontalBox> horizontalBox = SNew(SHorizontalBox);

		CheckerboardVerticalBox->AddSlot()
		.AutoHeight()
		[
			horizontalBox.ToSharedRef()
		];

		for (int x = -1; x < Checkerboard->DrawSize.X; ++x)
		{
			int curX = startLocation_x + x;
			int curY = startLocation_y + y;

			//头数
			if (y == -1 && x != -1)
			{
				if (x == 0)
				{
					horizontalBox->AddSlot()
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(Checkerboard->size_x)
						.HeightOverride(Checkerboard->size_y)
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
					.WidthOverride(Checkerboard->size_x + 8)
					.HeightOverride(Checkerboard->size_y)
					.VAlign(VAlign_Bottom)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::FromInt(curX)))
					]
				];

			}

			//旁数
			if (y != -1 && x == -1)
			{
				horizontalBox->AddSlot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(Checkerboard->size_x)
					.HeightOverride(Checkerboard->size_y)
					.HAlign(HAlign_Right)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::FromInt(curY)))
					]
				];
			}

			//内容
			if (y != -1 && x != -1)
			{
				TSharedPtr<SBorder> border = SNew(SBorder);
				FCookieLandLocation Location(curX, curY, SelectMapCube->SelectLocation.Z);

				Checkerboard->Location2Border.Add(Location.ToVector(), border);

				horizontalBox->AddSlot()
				.AutoWidth()
				[
					border.ToSharedRef()
				];

				DrawUpdateChessPiece(Location);
			}

		}
	}


	return CheckerboardVerticalBox;
}

void UCookieLandMapEditorView::DrawUpdateChessPiece(FCookieLandLocation PieceLocation)
{
	FVector PieceLocationVector = PieceLocation.ToVector();
	TSharedPtr<SBorder>* borderPtr = Checkerboard->Location2Border.Find(PieceLocationVector);
	if (!borderPtr) 
	{
		return;
	}
	TSharedPtr<SBorder> border = *borderPtr;

	FSlateColor PieceColor = HasMapBuildDataByLocation(PieceLocation) ? Checkerboard->BgHasDataColor : Checkerboard->BgEmptyColor;

	bool bIsSelect = SelectMapCube->SelectLocation == PieceLocation;

	TSharedPtr<SBox> box = SNew(SBox)
	.WidthOverride(Checkerboard->size_x)
	.HeightOverride(Checkerboard->size_y)
	[
		SNew(SOverlay)

		+ SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderImage(bIsSelect ? new FSlateRoundedBoxBrush(PieceColor, 6.0f, Checkerboard->SelectColor, 3.f) : new FSlateRoundedBoxBrush(PieceColor, 6.0f))
		]

		+ SOverlay::Slot()
		[
			SNew(SButton)
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
				.OnClicked_Lambda([this, PieceLocation]() {
					ChessPieceButtonClickCallback(PieceLocation);
				return FReply::Handled();
					})
				[
					Draw_ChessPieceContent(PieceLocation).ToSharedRef()
				]
		]
	];

	border->SetContent(
	SNew(SBorder)
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		box.ToSharedRef()
	]);
}


TSharedPtr<SHorizontalBox> UCookieLandMapEditorView::Draw_ChessPieceContent(FCookieLandLocation PieceLocation)
{
	TSharedPtr<SHorizontalBox> HBox = SNew(SHorizontalBox);

	/*if (pieceConfigData)
	{
		FName iconName;
		if (GlobalWidgetInfo->GetIsFinishPiece(pieceConfigData->Id))
		{
			iconName = "Icons.advanced";
		}
		else if (GlobalWidgetInfo->GetIsStartPiece(pieceConfigData->Id))
		{
			iconName = "Icons.animation";
		}
		else if (GlobalWidgetInfo->GetEnableMove(pieceConfigData, EPieceDirection::Up))
		{
			iconName = "Icons.arrow-up";
		}
		else if (GlobalWidgetInfo->GetEnableMove(pieceConfigData, EPieceDirection::Down))
		{
			iconName = "Icons.arrow-down";
		}

		if (!iconName.IsNone())
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

		if (!pieceConfigData->BaseInfo->Distance.IsUnLimit)
		{
			HBox->AddSlot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
						.Text(FText::FromString(FString::FromInt(pieceConfigData->BaseInfo->Distance.LimitDistance)))
				];
		}
	}*/

	return HBox;
}

bool UCookieLandMapEditorView::HasMapBuildDataByLocation(FCookieLandLocation PieceLocation)
{
	if (MapBuildActor)
	{
		return MapBuildActor->GetMapBuilder()->GetIsCubeOccupyByLocation(PieceLocation);
	}
	return false;
}

bool UCookieLandMapEditorView::HasPieceDataByLocation(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation)
{
	if (MapBuildActor)
	{
		return MapBuildActor->GetMapBuilder()->GetIsPieceOccupyByLocation(PieceLocation, PieceOrientation);
	}
	return false;
}

UCookieLandPiece* UCookieLandMapEditorView::GetPiece(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation)
{
	if (MapBuildActor)
	{
		return MapBuildActor->GetMapActorGather()->GetPiece(PieceLocation, PieceOrientation);
	}

	return nullptr;
}

void UCookieLandMapEditorView::ChessPieceButtonClickCallback(FCookieLandLocation PieceLocation)
{
	FVector OldSelect = SelectMapCube->SelectLocation;
	SelectMapCube->SelectLocation = PieceLocation.ToVector();

	DrawUpdateChessPiece(OldSelect);
	DrawUpdateChessPiece(SelectMapCube->SelectLocation);
	DrawUpdateSelectContext();
}

void UCookieLandMapEditorView::DrawUpdateSelectContext()
{
	SelectMapCube->SelectContextVerticalBox->ClearChildren();


	SelectMapCube->SelectContextVerticalBox->AddSlot()
	.AutoHeight()
	.Padding(0,0,0,10.f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(SelectMapCube->SelectLocation.ToString()))
	];

	SelectMapCube->SelectContextVerticalBox->AddSlot()
	.AutoHeight()
	[
		SNew(SBorder)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			Draw_Orientation().ToSharedRef()
		]
	];

	Checkerboard->SelectPieceContextVerticalBox = SNew(SVerticalBox);

	SelectMapCube->SelectContextVerticalBox->AddSlot()
	.Padding(0,10,0,0)
	.AutoHeight()
	[
		Checkerboard->SelectPieceContextVerticalBox.ToSharedRef()
	];
	DrawUpdateSelectPieceContext();
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_Orientation()
{
	TArray<int> OrientationValues = { -1,0,-1,-1,4,-1,2,-1,3,-1,5,-1, -1,1,-1 };

	TSharedPtr<SVerticalBox> OrientationVerticalBox = SNew(SVerticalBox);

	Checkerboard->Orientation2Border.Empty();

	for (int y = 0; y < 5; ++y)
	{
		TSharedPtr<SHorizontalBox> horizontalBox = SNew(SHorizontalBox);

		for (int x = 0; x < 3; ++x)
		{
			int Index = x + y * 3;
			int Value = OrientationValues[Index];
			if (Value != -1)
			{
				ECookieLandPieceOrientation Orientation = (ECookieLandPieceOrientation)(Value);

				TSharedPtr<SBorder> Border = SNew(SBorder);

				Checkerboard->Orientation2Border.Add(Orientation, Border);

				horizontalBox->AddSlot()
				.AutoWidth()
				[
					Border.ToSharedRef()
				];

				DrawUpdateSelectOrientation(Orientation);
			}
			else
			{
				horizontalBox->AddSlot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("				"))
				];
			}
		}

		OrientationVerticalBox->AddSlot()
		.AutoHeight()
		[
			horizontalBox.ToSharedRef()
		];
	}

	return OrientationVerticalBox;
}

void UCookieLandMapEditorView::DrawUpdateSelectOrientation(ECookieLandPieceOrientation Orientation)
{
	TSharedPtr<SBorder>* BorderPtr = Checkerboard->Orientation2Border.Find(Orientation);
	if (!BorderPtr)
	{
		return;
	}
	TSharedPtr<SBorder> Border = *BorderPtr;

	TMap<ECookieLandPieceOrientation, FString> OrientationNames = {
	{ECookieLandPieceOrientation::Up,"/\\"},{ECookieLandPieceOrientation::Down,"\\/"},{ECookieLandPieceOrientation::Left,"<"},{ECookieLandPieceOrientation::Right,">"},{ECookieLandPieceOrientation::Forward,"F"},{ECookieLandPieceOrientation::Backward,"B"}
	};
	FString OrientationName = OrientationNames[Orientation];

	bool bIsSelect = SelectMapCube->SelectOrientation == Orientation;
	FSlateColor PieceColor = HasPieceDataByLocation(SelectMapCube->SelectLocation, Orientation) ? Checkerboard->BgHasDataColor : Checkerboard->BgEmptyColor;


	TSharedPtr<SBox> box = SNew(SBox)
		.WidthOverride(Checkerboard->size_x)
		.HeightOverride(Checkerboard->size_y)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			[
				SNew(SBorder)
				.BorderImage(bIsSelect ? new FSlateRoundedBoxBrush(PieceColor, 6.0f, Checkerboard->SelectColor, 3.f) : new FSlateRoundedBoxBrush(PieceColor, 6.0f))
			]

			+ SOverlay::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
				.Text(FText::FromString(OrientationName))
				.OnClicked_Lambda([this, Orientation]() {
					OrientationButtonClickCallback(Orientation);
					return FReply::Handled();
				})
			]
		];

	Border->SetContent(
		SNew(SBorder)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			box.ToSharedRef()
		]);
}

void UCookieLandMapEditorView::OrientationButtonClickCallback(ECookieLandPieceOrientation PieceOrientation)
{
	ECookieLandPieceOrientation LastOrientation = SelectMapCube->SelectOrientation;
	SelectMapCube->SelectOrientation = PieceOrientation;

	DrawUpdateSelectOrientation(LastOrientation);
	DrawUpdateSelectOrientation(PieceOrientation);

	DrawUpdateSelectPieceContext();
}

void UCookieLandMapEditorView::DrawUpdateSelectPieceContext()
{
	TSharedPtr<SVerticalBox> PieceContextVerticalBox = Checkerboard->SelectPieceContextVerticalBox;

	PieceContextVerticalBox->ClearChildren();

	bool bHasData = HasPieceDataByLocation(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);
	UCookieLandPiece* Piece = GetPiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

	// 强制连接
	PieceContextVerticalBox->AddSlot()
	.Padding(0, 0, 0, 10)
	.AutoHeight()
	[
		Draw_ForceLinkContext().ToSharedRef()
	];

	if (!bHasData || Piece == nullptr)
	{
		PieceContextVerticalBox->AddSlot()
		.Padding(0, 0, 0, 10)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Create Piece"))
			.OnClicked_Lambda([this]() {
				CreatePieceButtonClickCallback();
				return FReply::Handled();
				})
		];
	}
	else
	{
		PieceContextVerticalBox->AddSlot()
		.Padding(0, 0, 0, 10)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Delete Piece"))
			.OnClicked_Lambda([this]() {
				DeletePieceButtonClickCallback();
				return FReply::Handled();
				})
		];

		PieceContextVerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("ActorType:"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SObjectPropertyEntryBox)
				.ObjectPath_Lambda([this, Piece]()
					{
						if (Piece->GetBuildInfo().PieceActorType)
						{
							return  Piece->GetBuildInfo().PieceActorType->GetPathName();
						}
						return FString();
					})
				.AllowedClass(ACookieLandPieceActor::StaticClass())
				.NewAssetFactories(TArray<UFactory*>())
				.OnObjectChanged_Lambda([this](const FAssetData& AssetData){
					PieceActorTypeChangeCallback(AssetData);
					})
			]
		];
	}
}

void UCookieLandMapEditorView::CreatePieceButtonClickCallback()
{
	if (MapBuildActor)
	{
		MapBuildActor->CreatePiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

		DrawUpdateChessPiece(SelectMapCube->SelectLocation);
		DrawUpdateSelectContext();
	}
}

void UCookieLandMapEditorView::DeletePieceButtonClickCallback()
{
	if (MapBuildActor)
	{
		MapBuildActor->DeletePiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);
		
		DrawUpdateChessPiece(SelectMapCube->SelectLocation);
		DrawUpdateSelectContext();
	}
}

void UCookieLandMapEditorView::PieceActorTypeChangeCallback(const FAssetData& AssetData)
{
	if (!MapBuildActor)
	{
		return;
	}

	UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *AssetData.ObjectPath.ToString());
	if (LoadedClass && LoadedClass->IsChildOf(ACookieLandPieceActor::StaticClass()))
	{
		MapBuildActor->ChangePieceActorType(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation, LoadedClass);
	}
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_ForceLinkContext()
{
	TSharedPtr<SVerticalBox> ForceLinkVerticalBox = SNew(SVerticalBox);

	FCookieLandOrientationLinkInfo ForceLinkInfo;
	if (MapBuildActor) 
	{
		MapBuildActor->GetMapBuilder()->GetForceLineInfo(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation, ForceLinkInfo);
	}

	Checkerboard->ForceLineNumberText = FText::FromString("0");

	if (ForceLinkInfo.GetIsValid())
	{
		ForceLinkVerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0, 0, 4, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(SelectMapCube->SelectLocation == ForceLinkInfo.Max_PieceLocation?"Cur ":"		"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString(FString::Printf(TEXT("%s"), *ForceLinkInfo.Max_PieceLocation.ToVector().ToString())))
				.OnClicked_Lambda([this, ForceLinkInfo]() {
					ChangeFloorButtonClickCallback(ForceLinkInfo.Max_PieceLocation.Floor - SelectMapCube->SelectLocation.Z);
					return FReply::Handled();
				})
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0, 0, 4, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(SelectMapCube->SelectLocation == ForceLinkInfo.Max_PieceLocation ? "Max" : ""))
			]
		];

		if (SelectMapCube->SelectLocation != ForceLinkInfo.Max_PieceLocation && SelectMapCube->SelectLocation != ForceLinkInfo.Min_PieceLocation)
		{
			ForceLinkVerticalBox->AddSlot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromString( "Cur " ))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString(FString::Printf(TEXT("%s"), *SelectMapCube->SelectLocation.ToString())))
					.OnClicked_Lambda([this, ForceLinkInfo]() {
						ChangeFloorButtonClickCallback(ForceLinkInfo.Max_PieceLocation.Floor - SelectMapCube->SelectLocation.Z);
						return FReply::Handled();
					})
				]
			];
		}
				
		ForceLinkVerticalBox->AddSlot()
		.AutoHeight()
		.Padding(0,0,0,10)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0,0,4,0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(SelectMapCube->SelectLocation == ForceLinkInfo.Min_PieceLocation?"Cur ":"		"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString(FString::Printf(TEXT("%s"), *ForceLinkInfo.Min_PieceLocation.ToVector().ToString())))
				.OnClicked_Lambda([this, ForceLinkInfo]() {
					ChangeFloorButtonClickCallback(ForceLinkInfo.Min_PieceLocation.Floor - SelectMapCube->SelectLocation.Z);
					return FReply::Handled();
				})
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(SelectMapCube->SelectLocation == ForceLinkInfo.Min_PieceLocation ? "Min" : ""))
			]
		];
	}

	bool bIsInEgde = ForceLinkInfo.GetIsValid() && (SelectMapCube->SelectLocation == ForceLinkInfo.Max_PieceLocation || SelectMapCube->SelectLocation == ForceLinkInfo.Min_PieceLocation);

	if (bIsInEgde || !ForceLinkInfo.GetIsValid())
	{
		ForceLinkVerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Force Line Num:"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4, 0, 0, 0)
			[
				SNew(SEditableTextBox)
				.Text(Checkerboard->ForceLineNumberText)
				.OnTextChanged_Lambda([this](const FText& InFilterText) {Checkerboard->ForceLineNumberText = InFilterText; })
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10,0,0,0)
			[
				SNew(SButton)
				.Text(FText::FromString("Update"))
				.OnClicked_Lambda([this]() {
					UpdateForceLineNumberButtonClickCallback();
					return FReply::Handled();
				})
			]
		];
	}

	if (bIsInEgde)
	{
		ForceLinkVerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Delete Force Link"))
			.OnClicked_Lambda([this]() {
				DeleteForceLinkButtonClickCallback();
				return FReply::Handled();
			})
		];
	}

	return ForceLinkVerticalBox;
}

void UCookieLandMapEditorView::UpdateForceLineNumberButtonClickCallback()
{
	if (!MapBuildActor)
	{
		return;
	}

	int UpdateForceLineNumber = FCString::Atoi(*Checkerboard->ForceLineNumberText.ToString());
	if (UpdateForceLineNumber == 0) { return; }

	FCookieLandLocation LineLocation(SelectMapCube->SelectLocation);
	LineLocation.AddDistanceByThreeDirection(SelectMapCube->SelectOrientation, UpdateForceLineNumber);

	MapBuildActor->GetMapBuilder()->PieceForceLine(SelectMapCube->SelectLocation, LineLocation, SelectMapCube->SelectOrientation);

	DrawUpdateSelectPieceContext();
}

void UCookieLandMapEditorView::DeleteForceLinkButtonClickCallback()
{
	if (!MapBuildActor)
	{
		return;
	}

	MapBuildActor->GetMapBuilder()->PieceDeleteForceLine(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

	DrawUpdateSelectPieceContext();
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_ViewControlContext()
{
	TSharedPtr<SVerticalBox> ViewControlVerticalBox = SNew(SVerticalBox);

	ViewControlVerticalBox->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(FString::Printf(TEXT("Floor:%d"), (int)SelectMapCube->SelectLocation.Z)))
		]

		+ SHorizontalBox::Slot()
		.Padding(10.f,0.f,0.f,0.f)
		.AutoWidth()
		[
			SNew(SButton)
			.Text(FText::FromString("/\\"))
			.OnClicked_Lambda([this]() {
				ChangeFloorButtonClickCallback(1);
				return FReply::Handled();
					})
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(FText::FromString("\\/"))
			.OnClicked_Lambda([this]() {
				ChangeFloorButtonClickCallback(-1);
				return FReply::Handled();
					})
		]
	];

	if (MapBuildActor)
	{
		ViewControlVerticalBox->AddSlot()
		.Padding(0,4,0,0)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Has Data Floor:"))
		];

		TArray<int> OccupyFloors = MapBuildActor->GetMapBuilder()->GetAllOccupyFloor();
		OccupyFloors.Sort([](const int32 A, const int32 B)
		{
			return A > B;
		});
		for (int Index = 0; Index < OccupyFloors.Num(); ++Index)
		{
			int Floor = OccupyFloors[Index];
			ViewControlVerticalBox->AddSlot()
			.AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(FString::Printf(TEXT("%d"), Floor)))
				.OnClicked_Lambda([this, Floor]() {
					ChangeFloorButtonClickCallback(Floor - SelectMapCube->SelectLocation.Z);
					return FReply::Handled();
					})
			];
		}
	}

	ViewControlVerticalBox->AddSlot()
	.Padding(0, 10, 0, 0)
	.AutoHeight()
	[
		SNew(SButton)
			.Text(FText::FromString("Delete Current Floor All Piece"))
			.OnClicked_Lambda([this]() {
			DeleteSelectFloorAllPieceButtonClickCallback();
			return FReply::Handled();
				})
	];

	ViewControlVerticalBox->AddSlot()
	.AutoHeight()
	[
		SNew(SButton)
			.Text(FText::FromString("Delete All Piece"))
			.OnClicked_Lambda([this]() {
			DeleteAllPieceButtonClickCallback();
			return FReply::Handled();
				})
	];

	ViewControlVerticalBox->AddSlot()
	.Padding(0, 10, 0, 0)
	.AutoHeight()
	[
		SNew(SButton)
		.Text(FText::FromString("Save MapBuildData"))
		.OnClicked_Lambda([this]() {
			SaveMapBuildDataToDataAsset();
			return FReply::Handled();
			})
	];


	return ViewControlVerticalBox;
}

void UCookieLandMapEditorView::ChangeFloorButtonClickCallback(int InAddValue)
{
	FVector Location = SelectMapCube->SelectLocation;
	Location.Z += InAddValue;
	SelectMapCube->SelectLocation = Location;
	DrawUpdateContentViewVerticalBox();
}

void UCookieLandMapEditorView::DeleteSelectFloorAllPieceButtonClickCallback()
{
	if (MapBuildActor)
	{
		MapBuildActor->DeleteTargetFloorAllPiece(SelectMapCube->SelectLocation.Z);
		DrawUpdateContentViewVerticalBox();
	}
}

void UCookieLandMapEditorView::DeleteAllPieceButtonClickCallback()
{
	if (MapBuildActor)
	{
		MapBuildActor->DeleteAllPiece();
		DrawUpdateContentViewVerticalBox();
	}
}

void UCookieLandMapEditorView::SaveMapBuildDataToDataAsset()
{
	if (!MapBuildActor)
	{
		return;
	}

	FCookieLandMapBuildDataTableRow MapBuildData;
	if (!UCookieLandMapBuildLibrary::GetSourceMapBuildDataTable(MapBuildActor->MapName, MapBuildData))
	{
		return;
	}

	if (MapBuildData.BuildDataAsset)
	{
		MapBuildData.BuildDataAsset->BuildInfo = MapBuildActor->MapBuildInfo;

		MapBuildData.BuildDataAsset->Modify();

		UPackage* package = MapBuildData.BuildDataAsset->GetOutermost();
		FEditorFileUtils::PromptForCheckoutAndSave({ package }, false, false);
	}

	if (MapBuildData.PiecesDataAsset)
	{
		TArray<FCookieLandPieceBuildInfo> PieceBuildInfos = MapBuildActor->GetAllPieceBuildInfo();
		MapBuildData.PiecesDataAsset->PieceBuildInfos = PieceBuildInfos;

		MapBuildData.PiecesDataAsset->Modify();

		UPackage* package = MapBuildData.PiecesDataAsset->GetOutermost();
		FEditorFileUtils::PromptForCheckoutAndSave({ package }, false, false);
	}

}

#undef LOCTEXT_NAMESPACE


#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
