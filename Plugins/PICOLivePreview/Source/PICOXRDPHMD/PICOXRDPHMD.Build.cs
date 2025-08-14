//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.
using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class PICOXRDPHMD : ModuleRules
	{
		public PICOXRDPHMD(ReadOnlyTargetRules Target) : base(Target)
		{
			string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
			
			PrivateIncludePaths.AddRange(
				new[] {
					"PICOXRDPHMD/Private",
					"PICOXRDPHMD/Public",
					EnginePath+"Source/Runtime/Renderer/Private",
				});

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"RHI",
					"RenderCore",
					"Renderer",
                    "InputCore",
					"InputDevice",
					"HeadMountedDisplay",
					"Slate",
					"SlateCore",
					"RHICore",
                    "XRBase"
                }
				);
            
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


                string PICOXRLibsDirectory = Path.Combine(ModuleDirectory, "../../Source/ThirdParty");

				PublicIncludePaths.Add(Path.Combine(PICOXRLibsDirectory, "Include"));

				PublicAdditionalLibraries.Add(Path.Combine(PICOXRLibsDirectory, "lib", "ps_base.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(PICOXRLibsDirectory, "lib", "ps_common.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(PICOXRLibsDirectory, "lib", "ps_driver_runtime.lib"));


				PublicDelayLoadDLLs.Add("ps_base.dll");
				PublicDelayLoadDLLs.Add("ps_common.dll");
				PublicDelayLoadDLLs.Add("ps_driver_runtime.dll");

				RuntimeDependencies.Add(Path.Combine(PICOXRLibsDirectory, "bin", "ps_base.dll"));
				RuntimeDependencies.Add(Path.Combine(PICOXRLibsDirectory, "bin", "ps_common.dll"));
				RuntimeDependencies.Add(Path.Combine(PICOXRLibsDirectory, "bin", "ps_driver_runtime.dll"));

			}
		}
	}
}
