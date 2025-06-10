// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CleverTapSampleEditorTarget : TargetRules
{
	public CleverTapSampleEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange( new string[] { "CleverTapSample" } );

#if UE_5_4_OR_LATER
	    DefaultBuildSettings = BuildSettingsVersion.V5;
#elif UE_5_3_OR_LATER 
	    DefaultBuildSettings = BuildSettingsVersion.V4;
#else
    	DefaultBuildSettings = BuildSettingsVersion.V2;
#endif

#if UE_5_1_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif 
	}
}
