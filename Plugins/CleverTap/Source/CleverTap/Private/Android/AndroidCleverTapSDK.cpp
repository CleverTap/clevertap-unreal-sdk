#include "Android/AndroidCleverTapSDK.h"

#include "CleverTapConfig.h"

namespace CleverTapSDK { namespace Android {

FCleverTapInstance* FPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
	return nullptr;
}

void FPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
}

} } // namespace CleverTapSDK::Android

