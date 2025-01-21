// Fill out your copyright notice in the Description page of Project Settings.


#include "CookieLandMapEditorView.h"
#include "CookieLandMapEditorStyle.h"
#include "CookieLand/Map/CookieLandMapBuildActor.h"
#include "CookieLand/Map/CookieLandPiece.h"
#include "CookieLand/Map/CookieLandMapBuildLibrary.h"
#include "CookieLand/Map/CookieLandMapBuilder.h"
#include "CookieLand/Map/CookieLandMapActorGather.h"
#include "CookieLand/Map/CookieLandMapSubsystem.h"
#include "CookieLand/PerceptualObject/CookieLandPerceptualObjectSubsystem.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "PropertyCustomizationHelpers.h"
#include "FileHelpers.h"
#include "UObject/NoExportTypes.h"
#include "IStructureDetailsView.h"

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
	PerceptualObject = NewObject<UCookieLandMapEditor_PerceptualObject>();

	PerceptualObject->MainPerceptualObjectEditor = NewObject<UCookieLandPerceptualObject>();

	Checkerboard->SelectColor = FAppStyle::Get().GetSlateColor("Colors.AccentFolder");
	Checkerboard->BgHasDataColor = FAppStyle::Get().GetSlateColor("Colors.SelectInactive");
	Checkerboard->BgEmptyColor = FAppStyle::Get().GetSlateColor("Colors.Header");

	UCookieLandMapSubsystem* MapSubsystem = UCookieLandMapBuildLibrary::GetMapSubsystem();
	MapBuildActor = MapSubsystem->GetMainMapBuildActor();
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
	SelectMapCube->SelectPieceDetailContextVerticalBox = SNew(SVerticalBox);

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
			SNew(SScrollBox)

			+ SScrollBox::Slot()
			[
				SNew(SBorder)
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						Draw_ViewControlContext().ToSharedRef()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0,10,0,0)
					[
						SelectMapCube->SelectContextVerticalBox.ToSharedRef()
					]
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4, 0)
		[
			SNew(SScrollBox)

			+ SScrollBox::Slot()
			[
				SNew(SBorder)
				[
					SelectMapCube->SelectPieceDetailContextVerticalBox.ToSharedRef()
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4,0)
		[
			SNew(SScrollBox)
			
			+ SScrollBox::Slot()
			[
				SNew(SBorder)
				[
					Draw_PerceptualObjectContext().ToSharedRef()
				]
			]
		]
			
	];

	DrawUpdateSelectContext();
	DrawUpdateSelectPieceDetailContext();

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

	Checkerboard->Location2VerticalBox.Empty();

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
				.Padding(2,0)
				[
					SNew(SBox)
					.WidthOverride(Checkerboard->size_x)
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
				.Padding(0,2)
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
				TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);
				FCookieLandLocation Location(curX, curY, SelectMapCube->SelectLocation.Z);

				Checkerboard->Location2VerticalBox.Add(Location.ToVector(), VerticalBox);

				horizontalBox->AddSlot()
				.AutoWidth()
				[
					VerticalBox.ToSharedRef()
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
	TSharedPtr<SVerticalBox>* VerticalBoxPtr = Checkerboard->Location2VerticalBox.Find(PieceLocationVector);
	if (!VerticalBoxPtr)
	{
		return;
	}
	TSharedPtr<SVerticalBox> VerticalBox = *VerticalBoxPtr;

	FSlateColor PieceColor = HasMapBuildDataByLocation(PieceLocation) ? Checkerboard->BgHasDataColor : Checkerboard->BgEmptyColor;

	bool bIsSelect = SelectMapCube->SelectLocation == PieceLocation;

	VerticalBox->ClearChildren();

	VerticalBox->AddSlot()
	.Padding(2,2)
	[
		SNew(SBox)
		.WidthOverride(Checkerboard->size_x)
		.HeightOverride(Checkerboard->size_y)
		[
			SNew(SBorder)
			.BorderImage(bIsSelect ? new FSlateRoundedBoxBrush(PieceColor, 6.0f, Checkerboard->SelectColor, 3.f) : new FSlateRoundedBoxBrush(PieceColor, 6.0f))
			[
				SNew(SButton)
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
				.OnClicked_Lambda([this, PieceLocation]() {
					ChessPieceButtonClickCallback(PieceLocation);
					return FReply::Handled();})
				[
					Draw_ChessPieceContent(PieceLocation).ToSharedRef()
				]
			]
		]
	];

}


TSharedPtr<SHorizontalBox> UCookieLandMapEditorView::Draw_ChessPieceContent(FCookieLandLocation PieceLocation)
{
	TSharedPtr<SHorizontalBox> HBox = SNew(SHorizontalBox);

	UCookieLandPiece* Piece = GetPiece(PieceLocation, SelectMapCube->SelectOrientation);
	if (Piece)
	{
		FName iconName;
		if (UCookieLandMapBuildLibrary::GetEnableMoveUpByMyself(Piece->GetBaseAttribute()))
		{
			iconName = "Icons.arrow-up";
		}
		if (UCookieLandMapBuildLibrary::GetEnableMoveDownByMyself(Piece->GetBaseAttribute()))
		{
			iconName = "Icons.arrow-down";
		}

		if (!iconName.IsNone())
		{
			HBox->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(FCookieLandMapEditorStyle::Get().GetBrush(iconName))
			];
		}
	}

	/*if (pieceConfigData)
	{
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
	if (IsValid(MapBuildActor))
	{
		return MapBuildActor->GetMapBuilder()->GetIsCubeOccupyByLocation(PieceLocation);
	}
	return false;
}

bool UCookieLandMapEditorView::HasPieceDataByLocation(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation)
{
	if (IsValid(MapBuildActor))
	{
		return MapBuildActor->GetMapBuilder()->GetIsPieceOccupyByLocator(PieceLocation, PieceOrientation);
	}
	return false;
}

UCookieLandPiece* UCookieLandMapEditorView::GetPiece(FCookieLandLocation PieceLocation, ECookieLandPieceOrientation PieceOrientation)
{
	if (IsValid(MapBuildActor))
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
	DrawUpdateSelectPieceDetailContext();
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
		.Padding(10.f, 0.f, 0.f, 0.f)
		.AutoWidth()
		[
			SNew(SButton)
			.Text(FText::FromString("/\\"))
			.OnClicked_Lambda([this]() {
				ChangeFloorButtonClickCallback(1);
				return FReply::Handled();})
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(FText::FromString("\\/"))
			.OnClicked_Lambda([this]() {
				ChangeFloorButtonClickCallback(-1);
				return FReply::Handled();})
		]
	];

	if (IsValid(MapBuildActor))
	{
		ViewControlVerticalBox->AddSlot()
		.Padding(0, 4, 0, 0)
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
	if (IsValid(MapBuildActor))
	{
		MapBuildActor->DeleteTargetFloorAllPiece(SelectMapCube->SelectLocation.Z);
		DrawUpdateContentViewVerticalBox();
	}
}

void UCookieLandMapEditorView::DeleteAllPieceButtonClickCallback()
{
	if (IsValid(MapBuildActor))
	{
		MapBuildActor->DeleteAllPiece();
		DrawUpdateContentViewVerticalBox();
	}
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

	SelectMapCube->SelectPieceContextVerticalBox = SNew(SVerticalBox);

	SelectMapCube->SelectPieceContextVerticalBox->AddSlot()
	.Padding(0,10,0,0)
	.AutoHeight()
	[
		SelectMapCube->SelectPieceContextVerticalBox.ToSharedRef()
	];
	DrawUpdateSelectPieceContext();
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_Orientation()
{
	TArray<int> OrientationValues = { -1,0,-1,-1,4,-1,2,-1,3,-1,5,-1, -1,1,-1 };

	TSharedPtr<SVerticalBox> OrientationVerticalBox = SNew(SVerticalBox);

	Checkerboard->Orientation2VerticalBox.Empty();

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

				TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

				Checkerboard->Orientation2VerticalBox.Add(Orientation, VerticalBox);

				horizontalBox->AddSlot()
				.AutoWidth()
				[
					VerticalBox.ToSharedRef()
				];

				DrawUpdateSelectOrientation(Orientation);
			}
			else
			{
				horizontalBox->AddSlot()
				.AutoWidth()
				[
					SNew(SBox)
					.Padding(2, 2)
					.WidthOverride(30)
					.HeightOverride(30)
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
	TSharedPtr<SVerticalBox>* VerticalBoxPtr = Checkerboard->Orientation2VerticalBox.Find(Orientation);
	if (!VerticalBoxPtr)
	{
		return;
	}
	TSharedPtr<SVerticalBox> VerticalBox = *VerticalBoxPtr;

	TMap<ECookieLandPieceOrientation, FString> OrientationNames = {
	{ECookieLandPieceOrientation::Up,"/\\"},{ECookieLandPieceOrientation::Down,"\\/"},{ECookieLandPieceOrientation::Left,"<"},{ECookieLandPieceOrientation::Right,">"},{ECookieLandPieceOrientation::Forward,"F"},{ECookieLandPieceOrientation::Backward,"B"}
	};
	FString OrientationName = OrientationNames[Orientation];

	bool bIsSelect = SelectMapCube->SelectOrientation == Orientation;
	FSlateColor PieceColor = HasPieceDataByLocation(SelectMapCube->SelectLocation, Orientation) ? Checkerboard->BgHasDataColor : Checkerboard->BgEmptyColor;

	VerticalBox->ClearChildren();

	VerticalBox->AddSlot()
	[
		SNew(SBox)
		.Padding(2,2)
		.WidthOverride(30)
		.HeightOverride(30)
		[
			SNew(SBorder)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.BorderImage(bIsSelect ? new FSlateRoundedBoxBrush(PieceColor, 6.0f, Checkerboard->SelectColor, 3.f) : new FSlateRoundedBoxBrush(PieceColor, 6.0f))
			[
				SNew(SButton)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
				.Text(FText::FromString(OrientationName))
				.OnClicked_Lambda([this, Orientation]() {
					OrientationButtonClickCallback(Orientation);
					return FReply::Handled(); })
			]
		]
	];
}

void UCookieLandMapEditorView::OrientationButtonClickCallback(ECookieLandPieceOrientation PieceOrientation)
{
	ECookieLandPieceOrientation LastOrientation = SelectMapCube->SelectOrientation;
	SelectMapCube->SelectOrientation = PieceOrientation;

	DrawUpdateSelectOrientation(LastOrientation);
	DrawUpdateSelectOrientation(PieceOrientation);

	DrawUpdateSelect();
}

void UCookieLandMapEditorView::DrawUpdateSelect()
{
	DrawUpdateSelectPieceContext();
	DrawUpdateSelectPieceDetailContext();
}

void UCookieLandMapEditorView::DrawUpdateSelectPieceContext()
{
	TSharedPtr<SVerticalBox> PieceContextVerticalBox = SelectMapCube->SelectPieceContextVerticalBox;

	PieceContextVerticalBox->ClearChildren();

	PieceContextVerticalBox->AddSlot()
	.Padding(0, 2)
	.AutoHeight()
	[
		SNew(SButton)
			.Text(FText::FromString("Set Initial Location"))
			.OnClicked_Lambda([this]() {
			SetToInitialLocationButtonClickCallback();
			return FReply::Handled(); })
	];

	PieceContextVerticalBox->AddSlot()
	.Padding(0, 2)
	.AutoHeight()
	[
		SNew(SButton)
			.Text(FText::FromString("Set Main Stand here"))
			.OnClicked_Lambda([this]() {
			SetMainPerceptualObjectStandHereButtonClickCallback();
			return FReply::Handled(); })
	];

	bool bHasData = HasPieceDataByLocation(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);
	UCookieLandPiece* Piece = GetPiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

	// 强制连接
	PieceContextVerticalBox->AddSlot()
	.Padding(0, 2)
	.AutoHeight()
	[
		Draw_ForceLinkContext().ToSharedRef()
	];

}

void UCookieLandMapEditorView::DrawUpdateSelectPieceDetailContext()
{
	TSharedPtr<SVerticalBox> PieceDetailContextVerticalBox = SelectMapCube->SelectPieceDetailContextVerticalBox;

	PieceDetailContextVerticalBox->ClearChildren();

	bool bHasData = HasPieceDataByLocation(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);
	UCookieLandPiece* Piece = GetPiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

	if (!bHasData || Piece == nullptr)
	{
		PieceDetailContextVerticalBox->AddSlot()
			.Padding(0, 2)
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
		PieceDetailContextVerticalBox->AddSlot()
			.Padding(0, 2)
			.AutoHeight()
			[
				SNew(SButton)
					.Text(FText::FromString("Delete Piece"))
					.OnClicked_Lambda([this]() {
					DeletePieceButtonClickCallback();
					return FReply::Handled(); })
			];

		FPropertyEditorModule& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FStructureDetailsViewArgs StructureViewArgs;
		StructureViewArgs.bShowObjects = true;
		StructureViewArgs.bShowAssets = true;
		StructureViewArgs.bShowClasses = true;
		StructureViewArgs.bShowInterfaces = true;

		FDetailsViewArgs ViewArgs;

		FCookieLandPieceBuildInfo PieceBuildInfo = Piece->GetBuildInfo();

		SelectMapCube->SelectPieceBuildInfoParameterValue = MakeShared<TStructOnScope<FCookieLandPieceBuildInfo>>(PieceBuildInfo);

		TSharedRef<class IStructureDetailsView> DetailsView = PropertyEditorModule.CreateStructureDetailView(ViewArgs, StructureViewArgs, nullptr);
		DetailsView->GetOnFinishedChangingPropertiesDelegate().AddUObject(this, &UCookieLandMapEditorView::PieceBuildInfoOnFinishedChangingPropertiesCallback);
		DetailsView->SetStructureData(SelectMapCube->SelectPieceBuildInfoParameterValue);

		PieceDetailContextVerticalBox->AddSlot()
			.Padding(0, 2)
			.AutoHeight()
			[
				DetailsView->GetWidget().ToSharedRef()
			];
	}
}

void UCookieLandMapEditorView::CreatePieceButtonClickCallback()
{
	if (IsValid(MapBuildActor))
	{
		MapBuildActor->CreatePiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

		DrawUpdateChessPiece(SelectMapCube->SelectLocation);
		DrawUpdateSelectContext();
		DrawUpdateSelect();
	}
}

void UCookieLandMapEditorView::DeletePieceButtonClickCallback()
{
	if (IsValid(MapBuildActor))
	{
		MapBuildActor->DeletePiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);
		
		DrawUpdateChessPiece(SelectMapCube->SelectLocation);
		DrawUpdateSelect();
	}
}

void UCookieLandMapEditorView::SetToInitialLocationButtonClickCallback()
{
	if (IsValid(MapBuildActor))
	{
		if (!MapBuildActor->MapBuildInfo.InitialLocation)
		{
			MapBuildActor->MapBuildInfo.InitialLocation = NewObject<UCookieLandVectorConfigInfo>();
		}
		MapBuildActor->MapBuildInfo.InitialLocation->Value = SelectMapCube->SelectLocation;
	}
}

void UCookieLandMapEditorView::SetMainPerceptualObjectStandHereButtonClickCallback()
{
	if (PerceptualObject->MainPerceptualObjectEditor)
	{
		PerceptualObject->MainPerceptualObjectEditor->PieceLocation = SelectMapCube->SelectLocation;
		PerceptualObject->MainPerceptualObjectEditor->PieceOrientation = SelectMapCube->SelectOrientation;

		UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();

		PerceptualObjectSubsystem->UpdatePerceptualObjectLocator(
			PerceptualObject->MainPerceptualObjectEditor->Id,
			FCookieLandPieceLocator(PerceptualObject->MainPerceptualObjectEditor->PieceLocation, PerceptualObject->MainPerceptualObjectEditor->PieceOrientation));

		DrawUpdateMainPerceptualObject();
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

void UCookieLandMapEditorView::PieceBuildInfoOnFinishedChangingPropertiesCallback(const FPropertyChangedEvent& PropertyChangedEvent)
{
	UCookieLandPiece* Piece = GetPiece(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);
	if (!Piece)
	{
		return;
	}

	FCookieLandPieceBuildInfo PieceBuildInfoEditor = *(SelectMapCube->SelectPieceBuildInfoParameterValue->Cast<FCookieLandPieceBuildInfo>());

	FProperty* MemberProperty = PropertyChangedEvent.MemberProperty;
	FString MemberPropertyName = MemberProperty->GetName();

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(FCookieLandPieceBuildInfo, PieceActorType))
	{
		MapBuildActor->ChangePieceActorType(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation, PieceBuildInfoEditor.PieceActorType);
	}
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(FCookieLandPieceBuildInfo, BaseAttribute))
	{
		Piece->SetBaseAttribute(PieceBuildInfoEditor.BaseAttribute);
		DrawUpdateChessPiece(SelectMapCube->SelectLocation);
	}
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(FCookieLandPieceBuildInfo, PieceActionDatas))
	{
		Piece->ClearPieceActions();

		for (int Index = 0; Index < PieceBuildInfoEditor.PieceActionDatas.Num(); ++Index)
		{
			UCookieLandBasePieceActionData* PieceActionData = PieceBuildInfoEditor.PieceActionDatas[Index];
			if (PieceActionData) 
			{
				Piece->AddPieceAction(PieceActionData);
			}
		}
	}

}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_ForceLinkContext()
{
	TSharedPtr<SVerticalBox> ForceLinkVerticalBox = SNew(SVerticalBox);

	FCookieLandOrientationLinkInfo ForceLinkInfo;
	if (IsValid(MapBuildActor)) 
	{
		MapBuildActor->GetMapBuilder()->GetForceLineInfo(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation, ForceLinkInfo);
	}

	SelectMapCube->ForceLineNumberText = FText::FromString("0");

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
				.Text(SelectMapCube->ForceLineNumberText)
				.OnTextChanged_Lambda([this](const FText& InFilterText) {SelectMapCube->ForceLineNumberText = InFilterText; })
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10,0,0,0)
			[
				SNew(SButton)
				.Text(FText::FromString("Confirm"))
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

	int UpdateForceLineNumber = FCString::Atoi(*SelectMapCube->ForceLineNumberText.ToString());
	if (UpdateForceLineNumber == 0) { return; }

	FCookieLandLocation LineLocation(SelectMapCube->SelectLocation);
	LineLocation.AddDistanceByThreeDirection(SelectMapCube->SelectOrientation, UpdateForceLineNumber);

	MapBuildActor->GetMapBuilder()->PieceForceLine(SelectMapCube->SelectLocation, LineLocation, SelectMapCube->SelectOrientation);

	DrawUpdateSelect();
}

void UCookieLandMapEditorView::DeleteForceLinkButtonClickCallback()
{
	if (!MapBuildActor)
	{
		return;
	}

	MapBuildActor->GetMapBuilder()->PieceDeleteForceLine(SelectMapCube->SelectLocation, SelectMapCube->SelectOrientation);

	DrawUpdateSelect();
}

TSharedPtr<SVerticalBox> UCookieLandMapEditorView::Draw_PerceptualObjectContext()
{
	TSharedPtr<SVerticalBox> PerceptualObjectVerticalBox = SNew(SVerticalBox);

	PerceptualObject->MainPerceptualObjectVerticalBox = SNew(SVerticalBox);
	PerceptualObjectVerticalBox->AddSlot()
	[
		PerceptualObject->MainPerceptualObjectVerticalBox.ToSharedRef()
	];
	DrawUpdateMainPerceptualObject();

	return PerceptualObjectVerticalBox;
}

void UCookieLandMapEditorView::DrawUpdateMainPerceptualObject()
{
	PerceptualObject->MainPerceptualObjectVerticalBox->ClearChildren();

	PerceptualObject->MainPerceptualObjectVerticalBox->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString("MainPerceptualObject:"))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4, 0, 0, 0)
		[
			SNew(SEditableTextBox)
			.Text(FText::FromName(PerceptualObject->MainPerceptualObjectType))
			.OnTextChanged_Lambda([this](const FText& InFilterText) {PerceptualObject->MainPerceptualObjectType =FName(InFilterText.ToString()); })
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(10, 0, 0, 0)
		[
			SNew(SButton)
			.Text(FText::FromString("Confirm"))
			.OnClicked_Lambda([this]() {
				CreateMainPerceptualObjectButtonClickCallback();
				return FReply::Handled();})
		]
	];

	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();

	PerceptualObject->MainPerceptualObjectEditor->Copy(PerceptualObjectSubsystem->GetMainPerceptualObject());

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs ViewArgs;

	TSharedRef<class IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(ViewArgs);
	DetailsView->OnFinishedChangingProperties().AddUObject(this, &UCookieLandMapEditorView::MainPerceptualObjectEditorChangePropertiesCallback);
	DetailsView->SetObject(PerceptualObject->MainPerceptualObjectEditor);

	PerceptualObject->MainPerceptualObjectVerticalBox->AddSlot()
	.AutoHeight()
	.Padding(0,4)
	[
		DetailsView
	];
}

void UCookieLandMapEditorView::MainPerceptualObjectEditorChangePropertiesCallback(const FPropertyChangedEvent& PropertyChangedEvent)
{
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();

	FProperty* MemberProperty = PropertyChangedEvent.MemberProperty;
	FString MemberPropertyName = MemberProperty->GetName();

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UCookieLandPerceptualObject, PieceLocation))
	{
		PerceptualObjectSubsystem->UpdatePerceptualObjectLocation(PerceptualObject->MainPerceptualObjectEditor->Id, PerceptualObject->MainPerceptualObjectEditor->PieceLocation);
	}
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UCookieLandPerceptualObject, PieceOrientation))
	{
		PerceptualObjectSubsystem->UpdatePerceptualObjectLocator(
			PerceptualObject->MainPerceptualObjectEditor->Id,
			FCookieLandPieceLocator(PerceptualObject->MainPerceptualObjectEditor->PieceLocation, PerceptualObject->MainPerceptualObjectEditor->PieceOrientation));
	}
}

void UCookieLandMapEditorView::CreateMainPerceptualObjectButtonClickCallback()
{
	UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
	const UCookieLandPerceptualObject* MainPerceptualObject = PerceptualObjectSubsystem->GetMainPerceptualObject();
	if (MainPerceptualObject && PerceptualObject->MainPerceptualObjectEditor && MainPerceptualObject->PerceptualObjectType == PerceptualObject->MainPerceptualObjectType)
	{
		return;
	}

	PerceptualObjectSubsystem->RemovePerceptualObject(PerceptualObject->MainPerceptualObjectEditor->Id);

	PerceptualObjectSubsystem->AddPerceptualObject(true, PerceptualObject->MainPerceptualObjectType, true);
	PerceptualObject->MainPerceptualObjectEditor->Copy(PerceptualObjectSubsystem->GetMainPerceptualObject());

	DrawUpdateMainPerceptualObject();
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
