// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ue5_demo : ModuleRules
{
	public ue5_demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "ProceduralMeshComponent" });

		CppStandard = CppStandardVersion.Cpp20;
	}
}
