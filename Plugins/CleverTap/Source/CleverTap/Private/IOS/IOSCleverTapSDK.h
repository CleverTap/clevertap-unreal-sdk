// Copyright CleverTap All Rights Reserved.
#pragma once

#include "GenericPlatformCleverTapSDK.h"

namespace CleverTapSDK { namespace IOS {

struct FPlatformSDK : GenericPlatform::FGenericPlatformSDK
{
	static void SetLogLevel(ECleverTapLogLevel Level);
	static UCleverTapInstance* InitializeSharedInstance(const FCleverTapInstanceConfig& Config);
	static UCleverTapInstance* InitializeSharedInstance(
		const FCleverTapInstanceConfig& Config, const FString& CleverTapId);
	static void SetRemoteNotificationToken(UCleverTapInstance& Instance, const TArray<uint8>& Token);
};

}} // namespace CleverTapSDK::IOS
