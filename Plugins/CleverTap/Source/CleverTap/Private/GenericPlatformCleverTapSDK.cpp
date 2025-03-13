// Copyright CleverTap All Rights Reserved.
#include "GenericPlatformCleverTapSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapUtilities.h"
#include "NullCleverTapInstance.h"

namespace CleverTapSDK { namespace GenericPlatform {

void FGenericPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	CleverTapSDK::Ignore(Level);
}

TUniquePtr<ICleverTapInstance> FGenericPlatformSDK::InitializeSharedInstance(
	const UCleverTapConfig& Config
)
{
	CleverTapSDK::Ignore(Config);
	return MakeUnique<FNullCleverTapInstance>();
}

TUniquePtr<ICleverTapInstance> FGenericPlatformSDK::InitializeSharedInstance(
	const UCleverTapConfig& Config, const FString& CleverTapId
)
{
	CleverTapSDK::Ignore(Config, CleverTapId);
	return MakeUnique<FNullCleverTapInstance>();
}

} } // namespace CleverTapSDK::GenericPlatform

