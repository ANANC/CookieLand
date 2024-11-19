// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CookieLand : ModuleRules
{
	public CookieLand(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        // 指定公共头文件目录
        PublicIncludePaths.AddRange(new string[] {
                 "CookieLand/Map/Public",
                 "CookieLand/Global/Public",
             });

        // 指定私有头文件目录
        PrivateIncludePaths.AddRange(new string[] {
                 "CookieLand/Map/Private",
                 "CookieLand/Global/Private",
             });
    }
}
