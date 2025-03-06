#include "GenericPlatformCleverTapSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapUtilities.h"

namespace CleverTapSDK { namespace GenericPlatform {

FCleverTapInstance* FGenericPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
	CleverTapSDK::Ignore(Config);
	return nullptr;
}

void FGenericPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
	CleverTapSDK::Ignore(Inst);
}

} } // namespace CleverTapSDK::GenericPlatform

