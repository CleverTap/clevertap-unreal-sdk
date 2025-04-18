// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CleverTapSampleTarget : TargetRules
{
	public CleverTapSampleTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "CleverTapSample" } );

		BuildEnvironment = TargetBuildEnvironment.Unique;
		bUseLoggingInShipping = true;
	}
}
