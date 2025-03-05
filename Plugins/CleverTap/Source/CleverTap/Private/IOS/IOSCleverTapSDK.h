// Copyright CleverTap All Rights Reserved.

#pragma once

#include "GenericPlatformCleverTapSDK.h"

namespace CleverTap { namespace IOS {

class FPlatformInstance
{ };

struct FPlatformSDK : GenericPlatform::FGenericPlatformSDK
{
	static FCleverTapInstance* TryInitializeCommonInstance(const UCleverTapConfig& Config);
	static void DestroyInstance(FCleverTapInstance& Inst);
};

} } // namespace CleverTap::IOS

