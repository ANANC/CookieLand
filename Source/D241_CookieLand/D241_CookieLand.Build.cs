// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class D241_CookieLand : ModuleRules
{
	public D241_CookieLand(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
