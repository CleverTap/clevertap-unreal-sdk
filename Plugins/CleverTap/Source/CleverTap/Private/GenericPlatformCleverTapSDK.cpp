#include "GenericPlatformCleverTapSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapUtilities.h"

namespace CleverTap { namespace GenericPlatform {

FCleverTapInstance* FGenericPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
	CleverTap::Ignore(Config);
	return nullptr;
}

void FGenericPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
	CleverTap::Ignore(Inst);
}

} } // namespace CleverTap::GenericPlatform

