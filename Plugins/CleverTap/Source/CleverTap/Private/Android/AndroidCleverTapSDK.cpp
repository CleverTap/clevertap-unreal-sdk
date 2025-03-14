// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"

#include "CleverTapInstance.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapUtilities.h"

namespace {

class FAndroidCleverTapInstance : public ICleverTapInstance
{
public:
	FString GetCleverTapId() const override
	{
		return FString{};
	}
};

} // namespace (anonymous)

namespace CleverTapSDK { namespace Android {

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	CleverTapSDK::Ignore(Level);
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config
)
{
	CleverTapSDK::Ignore(Config);
	return MakeUnique<FAndroidCleverTapInstance>();
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId
)
{
	CleverTapSDK::Ignore(Config, CleverTapId);
	return MakeUnique<FAndroidCleverTapInstance>();
}

} } // namespace CleverTapSDK::Android

