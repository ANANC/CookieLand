// Fill out your copyright notice in the Description page of Project Settings.


#include "Detail/CookieLandMapBuildActorDetail.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "CookieLand/Map/Public/CookieLandMapBuildActor.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "CookieLand/Map/Public/CookieLandMapSubsystem.h"
#include "CookieLand/Map/Public/CookieLandMapBuildLibrary.h"
#include "CookieLand/PerceptualObject/Public/CookieLandPerceptualObjectSubsystem.h"
#include "CookieLand/Map/Public/CookieLandMapBuilder.h"

#define LOCTEXT_NAMESPACE "CookieLandMapBuildActorDetail"  

void FCookieLandMapBuildActorDetail::CustomizeDetails(IDetailLayoutBuilder& InDetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> EditObjects;
	InDetailBuilder.GetObjectsBeingCustomized(EditObjects);

	if (EditObjects.Num() == 0 || !EditObjects[0].IsValid())
	{
		return;
	}
	for (TWeakObjectPtr<UObject> Object : EditObjects)
	{
		if (Object->IsA<ACookieLandMapBuildActor>())
		{
			ModifierInstance = Cast<ACookieLandMapBuildActor>(Object.Get());
		}
	}

	if (!ModifierInstance)
	{
		return;
	}

	IDetailCategoryBuilder& CategoryBuilder = InDetailBuilder.EditCategory(TEXT("Map"), LOCTEXT("MapSection", "Map"), ECategoryPriority::Default);
	CategoryBuilder.AddCustomRow(LOCTEXT("MapEditorLabel", "Control"))
		.WholeRowContent()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoWidth()
			[
				SNew(SButton)
					.Text(LOCTEXT("ReloadMapBuildInfoLabel", "加载地形数据（从表中）"))
					.OnClicked(this, &FCookieLandMapBuildActorDetail::ReloadMapBuildInfoButtonClick)
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("CreateEnvironmentLabel", "创建环境"))
				.OnClicked(this, &FCookieLandMapBuildActorDetail::CreateEnvironmentButtonClick)
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("UpdateAllPieceInstanceLocationLabel", "打开地形编辑界面"))
				.OnClicked(this, &FCookieLandMapBuildActorDetail::OpenMapBuilderEditorViewButtonClick)
			]

		];
}

FReply FCookieLandMapBuildActorDetail::ReloadMapBuildInfoButtonClick()
{
	if (ModifierInstance) 
	{
		ModifierInstance->ReloadMapBuildInfo();
	}

	return FReply::Handled();
}

FReply FCookieLandMapBuildActorDetail::CreateEnvironmentButtonClick()
{
	if (ModifierInstance)
	{
		ModifierInstance->CreateEnvironment();
	}
	
	return FReply::Handled();
}

FReply FCookieLandMapBuildActorDetail::OpenMapBuilderEditorViewButtonClick()
{
#if WITH_EDITOR
	if (ModifierInstance)
	{
		UCookieLandPerceptualObjectSubsystem* PerceptualObjectSubsystem = UCookieLandMapBuildLibrary::GetPerceptualObjectSubsystem();
		PerceptualObjectSubsystem->PerceptualObjectFindForceLinkInfoEvent.BindUObject(ModifierInstance->GetMapBuilder(), &UCookieLandMapBuilder::ExecutePerceptualObjectFindForceLinkInfoEventCallback);

		UCookieLandMapSubsystem* MapSubsystem = UCookieLandMapBuildLibrary::GetMapSubsystem();
		MapSubsystem->RegisterMapBuildActor(ModifierInstance);

		ModifierInstance->EnterThisMap(true);

		FName TabName("CookieLandMapEditor");
		FGlobalTabmanager::Get()->TryInvokeTab(TabName);
	}
#endif

	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE