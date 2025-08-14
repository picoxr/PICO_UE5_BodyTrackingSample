//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

using UnrealBuildTool;
using System.IO;

public class PICOXRDPInput : ModuleRules
{
	public PICOXRDPInput(ReadOnlyTargetRules Target) : base(Target)
	{
		string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
		System.Console.WriteLine(" Build the PICOXRDPInput Plugin");
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePathModuleNames.AddRange(
				new[]
				{
					"InputDevice",			// For IInputDevice.h
					"HeadMountedDisplay",	// For IMotionController.h
					"ImageWrapper",
				});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
					"Core",
					"CoreUObject",
					"ApplicationCore",
					"Engine",
					"InputCore",
					"HeadMountedDisplay",
					"PICOXRDPHMD",
					"RHICore",
					"XRBase"
			});
		PrivateIncludePaths.AddRange(
				new[] {
					"PICOXRDPInput/Private",
					"PICOXRDPHMD/Private",
				});

		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}

		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
		{
			AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
		}
		
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
							EnginePath+"Source/Runtime/Windows/D3D11RHI/Private",
							EnginePath+"Source/Runtime/Windows/D3D11RHI/Private/Windows",
							EnginePath+"Source/Runtime/D3D12RHI/Private",
							EnginePath+"Source/Runtime/D3D12RHI/Private/Windows",
				});

			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelExtensionsFramework");
		}
	}
}
