// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CleverTapSampleEditorTarget : TargetRules
{
	public CleverTapSampleEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange( new string[] { "CleverTapSample" } );

#if UE_5_0_OR_LATER
		DefaultBuildSettings = BuildSettingsVersion.V5;
		CppStandard = CppStandardVersion.Cpp20;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#else	
		DefaultBuildSettings = BuildSettingsVersion.V2;
#endif
	}
}
