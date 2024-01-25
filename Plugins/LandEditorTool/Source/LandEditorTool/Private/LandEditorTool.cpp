// Copyright Epic Games, Inc. All Rights Reserved.

#include "LandEditorTool.h"
#include "LandEditorToolStyle.h"
#include "LandEditorToolCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "LandEditorTool/Widget/LandEditorWidget.h"

static const FName LandEditorToolTabName("LandEditorTool");

#define LOCTEXT_NAMESPACE "FLandEditorToolModule"

void FLandEditorToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLandEditorToolStyle::Initialize();
	FLandEditorToolStyle::ReloadTextures();

	FLandEditorToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLandEditorToolCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FLandEditorToolModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLandEditorToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LandEditorToolTabName, FOnSpawnTab::CreateRaw(this, &FLandEditorToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FLandEditorToolTabTitle", "LandEditorTool"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FLandEditorToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLandEditorToolStyle::Shutdown();

	FLandEditorToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LandEditorToolTabName);
}

TSharedRef<SDockTab> FLandEditorToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	ULandEditorWidget* landEditorWidget = NewObject<ULandEditorWidget>();
	TSharedRef<SDockTab> widget = landEditorWidget->GetWidget();

	return widget;
}

void FLandEditorToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(LandEditorToolTabName);
}

void FLandEditorToolModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FLandEditorToolCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLandEditorToolCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLandEditorToolModule, LandEditorTool)