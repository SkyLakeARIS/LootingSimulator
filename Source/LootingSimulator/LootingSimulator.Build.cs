// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LootingSimulator : ModuleRules
{
	public LootingSimulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
        });

		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd"
			});
		}


PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"LootingSimulator",
			"LootingSimulator/Variant_Horror",
			"LootingSimulator/Variant_Horror/UI",
			"LootingSimulator/Variant_Shooter",
			"LootingSimulator/Variant_Shooter/AI",
			"LootingSimulator/Variant_Shooter/UI",
			"LootingSimulator/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
