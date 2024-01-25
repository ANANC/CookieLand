// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LandEditorToolStyle.h"

class FLandEditorToolCommands : public TCommands<FLandEditorToolCommands>
{
public:

	FLandEditorToolCommands()
		: TCommands<FLandEditorToolCommands>(TEXT("LandEditorTool"), NSLOCTEXT("Contexts", "LandEditorTool", "LandEditorTool Plugin"), NAME_None, FLandEditorToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};