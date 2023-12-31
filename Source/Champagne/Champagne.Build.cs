// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Champagne : ModuleRules
{
	public Champagne(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Niagara", "UMG", "CableComponent" });
	}
}
