// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class UESnowWorld : ModuleRules
{
	public UESnowWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", 
			"InputCore", "EnhancedInput",
        "AIModule",
        "NavigationSystem",
         "GameplayAbilities", "GameplayTags", "GameplayTasks"});
	}
    //"GameplayAbilities", "GameplayTags",
}
