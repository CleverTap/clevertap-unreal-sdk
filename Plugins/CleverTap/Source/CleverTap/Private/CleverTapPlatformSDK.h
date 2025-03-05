// Copyright CleverTap All Rights Reserved.

#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTap::Android::FPlatformSDK;
using FCleverTapPlatformInstance = CleverTap::Android::FPlatformInstance;

#elif PLATFORM_IOS
#include "IOS/IOSCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTap::IOS::FPlatformSDK;
using FCleverTapPlatformInstance = CleverTap::IOS::FPlatformInstance;

#else
#include "GenericPlatformCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTap::GenericPlatform::FGenericPlatformSDK;
using FCleverTapPlatformInstance = CleverTap::GenericPlatform::FPlatformInstance;
#endif

/**
 * A platform specific implementation of a CleverTap account instance object.
 */
class FCleverTapInstance : public FCleverTapPlatformInstance
{
public:
	using FCleverTapPlatformInstance::FCleverTapPlatformInstance;
};

