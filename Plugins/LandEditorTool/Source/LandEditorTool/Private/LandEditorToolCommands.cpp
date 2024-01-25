// Copyright Epic Games, Inc. All Rights Reserved.

#include "LandEditorToolCommands.h"

#define LOCTEXT_NAMESPACE "FLandEditorToolModule"

void FLandEditorToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "LandEditorTool", "Bring up LandEditorTool window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
