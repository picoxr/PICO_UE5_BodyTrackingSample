//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

using UnrealBuildTool;
using System.IO;

public class PICOXRDPEditor : ModuleRules
{
	public PICOXRDPEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivatePCHHeaderFile = "Public/PXR_DPEditor.h";

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        PrivateIncludePaths.AddRange(
	        new [] {
		        "PICOXRDPHMD/Private",
		        "PICOXRDPInput/Private"
	        });
        
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore",
                "ApplicationCore",
                "InputDevice",
                "BlueprintGraph",
                "AnimGraph",
				"ToolMenus",
				"PICOXRDPInput",
				"PICOXRDPHMD",
				"RHICore"
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"RenderCore",
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"PICOXRDPHMD",
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
		{
			AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
		}
		
		string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"D3D11RHI",
					"D3D12RHI",
				});

			PrivateIncludePaths.AddRange(
				new string[]
				{
					EnginePath + "Source/Runtime/Windows/D3D11RHI/Private",
					EnginePath + "Source/Runtime/Windows/D3D11RHI/Private/Windows",
					EnginePath + "Source/Runtime/D3D12RHI/Private",
					EnginePath + "Source/Runtime/D3D12RHI/Private/Windows",
				});

			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelExtensionsFramework");
		}
    }
}
