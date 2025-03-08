// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SingularisVehicle : ModuleRules
{
	public SingularisVehicle(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			[
			]
		);


		PrivateIncludePaths.AddRange(
			[
			]
		);


		PublicDependencyModuleNames.AddRange(
			[
				"Core"
			]
		);


		PrivateDependencyModuleNames.AddRange(
			[
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"EnhancedInput",
				"ChaosVehicles"
			]
		);


		DynamicallyLoadedModuleNames.AddRange(
			[
			]
		);
	}
}