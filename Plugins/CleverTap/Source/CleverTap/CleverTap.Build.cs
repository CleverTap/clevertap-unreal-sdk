// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using System.Text.RegularExpressions;
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
				"Json",
			}
		);
		
		if (Target.Type == TargetRules.TargetType.Editor)
		{
			PrivateDependencyModuleNames.Add("Settings");
		}
		
		// Plugin version as major * 10000 + minor * 100 + patch (1.4.0 -> 10400), read from CleverTap.uplugin so the
		// version lives in one place. The platform code registers it with the native SDK via setCustomSdkVersion.
		PrivateDefinitions.Add("CLEVERTAP_UNREAL_SDK_VERSION=" + ReadPluginVersionCode());

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

	private int ReadPluginVersionCode()
	{
		string UpluginPath = Path.Combine(PluginDirectory, "CleverTap.uplugin");
		Match VersionMatch = Regex.Match(File.ReadAllText(UpluginPath), "\"VersionName\"\\s*:\\s*\"(\\d+)\\.(\\d+)\\.(\\d+)\"");
		if (!VersionMatch.Success)
		{
			throw new BuildException("CleverTap: VersionName in {0} must be major.minor.patch", UpluginPath);
		}
		int Major = int.Parse(VersionMatch.Groups[1].Value);
		int Minor = int.Parse(VersionMatch.Groups[2].Value);
		int Patch = int.Parse(VersionMatch.Groups[3].Value);
		if (Minor > 99 || Patch > 99)
		{
			throw new BuildException("CleverTap: minor and patch in VersionName must be at most 99 ({0})", UpluginPath);
		}
		return Major * 10000 + Minor * 100 + Patch;
	}
}
