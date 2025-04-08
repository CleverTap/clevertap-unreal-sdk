// Copyright CleverTap All Rights Reserved.
#include "GenericPlatformCleverTapSDK.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapUtilities.h"
#include "NullCleverTapInstance.h"

namespace CleverTapSDK { namespace GenericPlatform {

void FGenericPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	CleverTapSDK::Ignore(Level);
}

TUniquePtr<ICleverTapInstance> FGenericPlatformSDK::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	CleverTapSDK::Ignore(Config);
	return MakeUnique<FNullCleverTapInstance>();
}

TUniquePtr<ICleverTapInstance> FGenericPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	CleverTapSDK::Ignore(Config, CleverTapId);
	return MakeUnique<FNullCleverTapInstance>();
}

void FGenericPlatformSDK::SetRemoteNotificationToken(ICleverTapInstance& Instance, const TArray<uint8>& Token)
{
	CleverTapSDK::Ignore(Instance, Token);
}

}} // namespace CleverTapSDK::GenericPlatform
