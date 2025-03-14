// Copyright CleverTap All Rights Reserved.
#pragma once

#include "GenericPlatformCleverTapSDK.h"

namespace CleverTapSDK { namespace IOS {

struct FPlatformSDK : GenericPlatform::FGenericPlatformSDK
{
	static void SetLogLevel(ECleverTapLogLevel Level);
	static TUniquePtr<ICleverTapInstance> InitializeSharedInstance(
		const FCleverTapInstanceConfig& Config
	);
	static TUniquePtr<ICleverTapInstance> InitializeSharedInstance(
		const FCleverTapInstanceConfig& Config, const FString& CleverTapId
	);
};

} } // namespace CleverTapSDK::IOS

