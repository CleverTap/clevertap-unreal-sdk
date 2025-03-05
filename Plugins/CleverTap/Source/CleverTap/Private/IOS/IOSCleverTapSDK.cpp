#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapConfig.h"

namespace CleverTap { namespace IOS {

FCleverTapInstance* FPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
	return nullptr;
}

void FPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
}

} } // namespace CleverTap::IOS

