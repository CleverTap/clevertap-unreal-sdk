#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapConfig.h"

#import <CleverTapSDK/CleverTap.h>

namespace CleverTapSDK { namespace IOS {

FCleverTapInstance* FPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
	[CleverTap autoIntegrate];
	return nullptr;
}

void FPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
}

} } // namespace CleverTapSDK::IOS

