// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class YH_True_FPS : ModuleRules
{
	public YH_True_FPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "RecoilAnimation", "AnimGraphRuntime" });

		// "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "RecoilAnimation"
    }
}
