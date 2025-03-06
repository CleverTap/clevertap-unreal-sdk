#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapConfig.h"

namespace CleverTapSDK { namespace IOS {

FCleverTapInstance* FPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
	return nullptr;
}

void FPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
}

} } // namespace CleverTapSDK::IOS

