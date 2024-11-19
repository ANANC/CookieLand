// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CookieLandMapEditor : ModuleRules
{
	public CookieLandMapEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CookieLand"
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "PropertyEditor",
				// ... add private dependencies that you statically link with here ...	
			}
			);

        // 指定公共头文件目录
        PublicIncludePaths.AddRange(new string[] {
                 "CookieLandMapEditor/Public",
             });

        // 指定私有头文件目录
        PrivateIncludePaths.AddRange(new string[] {
                 "CookieLandMapEditor/Private",
             });

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
