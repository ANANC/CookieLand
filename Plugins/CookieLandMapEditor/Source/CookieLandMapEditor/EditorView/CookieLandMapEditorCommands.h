// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CookieLandMapEditorStyle.h"

class FCookieLandMapEditorCommands : public TCommands<FCookieLandMapEditorCommands>
{
public:

	FCookieLandMapEditorCommands()
		: TCommands<FCookieLandMapEditorCommands>(TEXT("CookieLandMapEditor"), NSLOCTEXT("Contexts", "CookieLandMapEditor", "CookieLandMapEditor Plugin"), NAME_None, FCookieLandMapEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};