// Copyright CleverTap All Rights Reserved.
#pragma once

#if PLATFORM_ANDROID
	#include "Android/AndroidCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTapSDK::Android::FPlatformSDK;

#elif PLATFORM_IOS
	#include "IOS/IOSCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTapSDK::IOS::FPlatformSDK;

#else
	#include "GenericPlatformCleverTapSDK.h"

using FCleverTapPlatformSDK = CleverTapSDK::GenericPlatform::FGenericPlatformSDK;
#endif
