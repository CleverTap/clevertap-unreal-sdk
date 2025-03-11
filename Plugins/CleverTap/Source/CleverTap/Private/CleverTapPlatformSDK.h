// Copyright CleverTap All Rights Reserved.

#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTapSDK::Android::FPlatformSDK;
using FCleverTapPlatformInstance = CleverTapSDK::Android::FPlatformInstance;

#elif PLATFORM_IOS
#include "IOS/IOSCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTapSDK::IOS::FPlatformSDK;
using FCleverTapPlatformInstance = CleverTapSDK::IOS::FPlatformInstance;

#else
#include "GenericPlatformCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTapSDK::GenericPlatform::FGenericPlatformSDK;
using FCleverTapPlatformInstance = CleverTapSDK::GenericPlatform::FPlatformInstance;
#endif

/**
 * A platform specific implementation of a CleverTap account instance object.
 */
class FCleverTapInstance : public FCleverTapPlatformInstance
{
public:
	using FCleverTapPlatformInstance::FCleverTapPlatformInstance;
};

