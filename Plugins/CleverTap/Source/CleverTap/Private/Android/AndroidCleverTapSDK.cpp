#include "Android/AndroidCleverTapSDK.h"

#include "CleverTapConfig.h"

namespace CleverTap { namespace Android {

FCleverTapInstance* FPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
	return nullptr;
}

void FPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
}

} } // namespace CleverTap::Android

