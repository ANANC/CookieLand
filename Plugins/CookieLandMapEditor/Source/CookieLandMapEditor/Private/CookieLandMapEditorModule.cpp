// Copyright Epic Games, Inc. All Rights Reserved.

#include "CookieLandMapEditor/Public/CookieLandMapEditorModule.h"
#include "CookieLandMapEditor/Public/EditorView/CookieLandMapEditorStyle.h"
#include "CookieLandMapEditor/Public/EditorView/CookieLandMapEditorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "CookieLand/Map/Public/CookieLandMapBuildActor.h"
#include "Detail/CookieLandMapBuildActorDetail.h"
#include "CookieLandMapEditor/Public/EditorView/CookieLandMapEditorView.h"

static const FName CookieLandMapEditorTabName("CookieLandMapEditor");

#define LOCTEXT_NAMESPACE "FCookieLandMapEditorModule"

void FCookieLandMapEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FCookieLandMapEditorStyle::Initialize();
	FCookieLandMapEditorStyle::ReloadTextures();

	FCookieLandMapEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCookieLandMapEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FCookieLandMapEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCookieLandMapEditorModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(CookieLandMapEditorTabName, FOnSpawnTab::CreateRaw(this, &FCookieLandMapEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FCookieLandMapEditorTabTitle", "MapEditorView"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	{
		PropertyModule.RegisterCustomClassLayout(ACookieLandMapBuildActor::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCookieLandMapBuildActorDetail::MakeInstance));
	}
}

void FCookieLandMapEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FCookieLandMapEditorStyle::Shutdown();

	FCookieLandMapEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CookieLandMapEditorTabName);

	if (UObjectInitialized() && !IsEngineExitRequested())
	{
		FPropertyEditorModule* PropertyEditorModule = FModuleManager::Get().GetModulePtr<FPropertyEditorModule>("PropertyEditor");
		if (PropertyEditorModule)
		{
			PropertyEditorModule->UnregisterCustomClassLayout(ACookieLandMapBuildActor::StaticClass()->GetFName());
		}
	}
}

TSharedRef<SDockTab> FCookieLandMapEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	UCookieLandMapEditorView* MapEditorView = NewObject< UCookieLandMapEditorView>();
	if (MapEditorView)
	{
		MapEditorView->Init();
		return MapEditorView->Draw();
	}

	return SNew(SDockTab);
}

void FCookieLandMapEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(CookieLandMapEditorTabName);
}

void FCookieLandMapEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FCookieLandMapEditorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCookieLandMapEditorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCookieLandMapEditorModule, CookieLandMapEditor)