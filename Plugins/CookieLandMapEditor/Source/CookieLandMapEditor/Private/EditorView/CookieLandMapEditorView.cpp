// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorView/CookieLandMapEditorView.h"
#include "CookieLand/Map/Public/CookieLandMapBuildActor.h"
#include "CookieLand/Map/Public/CookieLandPiece.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"
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
		return MapBuildActor->GetIsCubeOccupyByLocation(PieceLocation);
	}
	return false;
}

bool UCookieLandMapEditorView::HasPieceDataByLocation(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation)
{
	if (MapBuildActor)
	{
		return MapBuildActor->GetIsPieceOccupyByLocation(PieceLocation, PieceOrientation);
	}
	return false;
}

UCookieLandPiece* UCookieLandMapEditorView::GetPiece(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation)
{
	if (MapBuildActor)
	{
		return MapBuildActor->GetPiece(PieceLocation, PieceOrientation);
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
	[
		Draw_Orientation().ToSharedRef()
	];

	SelectMapCube->SelectContextVerticalBox->AddSlot()
	.Padding(0,10,0,0)
	.AutoHeight()
	[
		Draw_SelectPieceContext().ToSharedRef()
	];
	
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_Orientation()
{
	TArray<int> OrientationValues = { -1,0,-1,-1,4,-1,2,-1,3,-1,5,-1, -1,1,-1 };
	TMap<ECookieLandPieceOrientation, FString> OrientationNames{
		{ECookieLandPieceOrientation::Up,"/\\"},{ECookieLandPieceOrientation::Down,"\\/"},{ECookieLandPieceOrientation::Left,"<"},{ECookieLandPieceOrientation::Right,">"},{ECookieLandPieceOrientation::Forward,"F"},{ECookieLandPieceOrientation::Backward,"B"}
	};

	TSharedPtr<SVerticalBox> OrientationVerticalBox = SNew(SVerticalBox);

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

				bool bIsSelect = SelectMapCube->SelectOrientation == Orientation;
				FSlateColor PieceColor = HasPieceDataByLocation(SelectMapCube->SelectLocation, Orientation) ? Checkerboard->BgHasDataColor : Checkerboard->BgEmptyColor;

				horizontalBox->AddSlot()
				.AutoWidth()
				.Padding(4, 4)
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
						.Text(FText::FromString(OrientationNames[Orientation]))
						.OnClicked_Lambda([this, Orientation]() {
							OrientationButtonClickCallback(Orientation);
							return FReply::Handled();
						})
					]
					
				];
			}
			else
			{
				horizontalBox->AddSlot()
				.AutoWidth()
				.Padding(4, 4)
				[
					SNew(SButton)
					.Text(FText::FromString("	"))
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

void UCookieLandMapEditorView::OrientationButtonClickCallback(ECookieLandPieceOrientation PieceOrientation)
{
	SelectMapCube->SelectOrientation = PieceOrientation;
	DrawUpdateSelectContext();
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_SelectPieceContext()
{
	TSharedPtr<SVerticalBox> PieceContextVerticalBox = SNew(SVerticalBox);

	bool bHasData = HasPieceDataByLocation(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);
	UCookieLandPiece* Piece = GetPiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

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

	return PieceContextVerticalBox;
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

		TArray<int> OccupyFloors = MapBuildActor->GetAllOccupyFloor();
		for (int Index = 0; Index < OccupyFloors.Num(); ++Index)
		{
			ViewControlVerticalBox->AddSlot()
			.AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(FString::Printf(TEXT("%d"), Index)))
				.OnClicked_Lambda([this, Index]() {
					ChangeFloorButtonClickCallback(Index - SelectMapCube->SelectLocation.Z);
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
