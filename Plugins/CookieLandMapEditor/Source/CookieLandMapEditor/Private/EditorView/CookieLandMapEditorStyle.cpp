// Copyright Epic Games, Inc. All Rights Reserved.

#include "CookieLandMapEditor/Public/EditorView/CookieLandMapEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FCookieLandMapEditorStyle::StyleInstance = nullptr;

void FCookieLandMapEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FCookieLandMapEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FCookieLandMapEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("CookieLandMapEditorStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FCookieLandMapEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("CookieLandMapEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("CookieLandMapEditor")->GetBaseDir() / TEXT("Resources"));

	Style->Set("CookieLandMapEditor.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FCookieLandMapEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FCookieLandMapEditorStyle::Get()
{
	return *StyleInstance;
}
