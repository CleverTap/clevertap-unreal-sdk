// Copyright CleverTap All Rights Reserved.
#include "GenericPlatformCleverTapSDK.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapUtilities.h"
#include "CleverTapSubsystem.h"
#include "NullCleverTapInstance.h"

namespace CleverTapSDK { namespace GenericPlatform {

void FGenericPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	CleverTapSDK::Ignore(Level);
}

UCleverTapInstance* FGenericPlatformSDK::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	CleverTapSDK::Ignore(Config);
	return UNullCleverTapInstance::Create();
}

UCleverTapInstance* FGenericPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	CleverTapSDK::Ignore(Config, CleverTapId);
	return UNullCleverTapInstance::Create();
}

void FGenericPlatformSDK::SetRemoteNotificationToken(UCleverTapInstance& Instance, const TArray<uint8>& Token)
{
	CleverTapSDK::Ignore(Instance, Token);
}

}} // namespace CleverTapSDK::GenericPlatform
