// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "CleverTapInstance.h"

enum class ECleverTapLogLevel : uint8;
struct FCleverTapInstanceConfig;

namespace CleverTapSDK { namespace GenericPlatform {

struct FGenericPlatformSDK
{
	/**
	 * Sets the the platform log level for the CleverTap SDK.
	 */
	static void SetLogLevel(ECleverTapLogLevel Level);

	/**
	 * Initialize the shared CleverTap instance. If successful this returns a non-null
	 *  instance object. This instance object can be used to call other SDK methods. It should
	 *  eventually be destroyed using the DestroyInstance(ICleverTapInstanceInterface&) method.
	 */
	static TUniquePtr<ICleverTapInstance> InitializeSharedInstance(const FCleverTapInstanceConfig& Config);

	/**
	 * Initialize the shared CleverTap instance with a custom CleverTap Id. If successful this
	 *  returns a non-null instance object. This instance object can be used to call other SDK methods.
	 *  It should eventually be destroyed using the DestroyInstance(ICleverTapInstanceInterface&) method.
	 */
	static TUniquePtr<ICleverTapInstance> InitializeSharedInstance(
		const FCleverTapInstanceConfig& Config, const FString& CleverTapId);
};

}} // namespace CleverTapSDK::GenericPlatform
