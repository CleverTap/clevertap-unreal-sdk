// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class CleverTap : ModuleRules
{
	public CleverTap(ReadOnlyTargetRules Target) : base(Target)
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
				// ... add other public dependencies that you statically link with here ...
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
		);
		
		if (Target.Type == TargetRules.TargetType.Editor)
		{
			PrivateDependencyModuleNames.Add("Settings");
		}
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
    		AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "CleverTap_Android_UPL.xml"));
		}


		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PrivateDefinitions.Add("CLEVERTAP_NO_INAPP_SUPPORT=0");

			string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "ThirdParty", "IOS");
			string CleverTapFrameworkPath = Path.Combine(ThirdPartyPath, "CleverTapSDK.framework.zip");
			string SDWebImageFrameworkPath = Path.Combine(ThirdPartyPath, "SDWebImage.framework.zip");
			PublicAdditionalFrameworks.Add(new Framework("CleverTapSDK", CleverTapFrameworkPath, bCopyFramework: true));
			PublicAdditionalFrameworks.Add(new Framework("SDWebImage", SDWebImageFrameworkPath, bCopyFramework: true));
			RuntimeDependencies.Add(CleverTapFrameworkPath);
			RuntimeDependencies.Add(SDWebImageFrameworkPath);

			string RelPluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(RelPluginPath, "CleverTap_IOS_UPL.xml"));
		}
	}
}
