// Copyright Epic Games, Inc. All Rights Reserved.

#include "CookieLandMapEditor/Public/EditorView/CookieLandMapEditorCommands.h"

#define LOCTEXT_NAMESPACE "FCookieLandMapEditorModule"

void FCookieLandMapEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "CookieLandMapEditor", "Bring up CookieLandMapEditor window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
