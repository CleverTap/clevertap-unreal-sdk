// Copyright CleverTap All Rights Reserved.
#pragma once

// Name this plugin registers with the native SDKs through setLibrary and setCustomSdkVersion.
// It appears in the "af" header of every request, next to the version.
#define CLEVERTAP_UNREAL_SDK_NAME "Unreal"

// CLEVERTAP_UNREAL_SDK_VERSION (major * 10000 + minor * 100 + patch) is defined by CleverTap.Build.cs
// from VersionName in CleverTap.uplugin.
#ifndef CLEVERTAP_UNREAL_SDK_VERSION
#define CLEVERTAP_UNREAL_SDK_VERSION 0
#endif
