// Copyright Epic Games, Inc. All Rights Reserved.

#include "LandEditorToolStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FLandEditorToolStyle::StyleInstance = nullptr;

void FLandEditorToolStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLandEditorToolStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLandEditorToolStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("LandEditorToolStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FLandEditorToolStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("LandEditorToolStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("LandEditorTool")->GetBaseDir() / TEXT("Resources"));

	Style->Set("LandEditorTool.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	// Icons
	Style->Set("Icons.advanced", new IMAGE_BRUSH_SVG("Advanced", Icon16x16));
	Style->Set("Icons.arrow-down", new IMAGE_BRUSH_SVG("arrow-down", Icon16x16));
	Style->Set("Icons.arrow-up", new IMAGE_BRUSH_SVG("arrow-up", Icon16x16));
	Style->Set("Icons.bullet-point", new IMAGE_BRUSH_SVG("bullet-point", Icon16x16));
	Style->Set("Icons.redo", new IMAGE_BRUSH_SVG("Redo", Icon16x16));
	Style->Set("Icons.animation", new IMAGE_BRUSH_SVG("Animation", Icon16x16));
	
	return Style;
}

void FLandEditorToolStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FLandEditorToolStyle::Get()
{
	return *StyleInstance;
}
