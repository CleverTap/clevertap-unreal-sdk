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
	 * Initializes the shared CleverTap instance with a custom CleverTap Id.
	 * Returns a non-null instance object if successful.
	 *
	 * The returned object is owned by the CleverTapSubsystem and does not need to be manually destroyed.
	 * You can safely store and use the pointer while the subsystem is active.
	 */
	static UCleverTapInstance* InitializeSharedInstance(const FCleverTapInstanceConfig& Config);

	/**
	 * Initializes the shared CleverTap instance with a custom CleverTap Id.
	 * Returns a non-null instance object if successful.
	 *
	 * The returned object is owned by the CleverTapSubsystem and does not need to be manually destroyed.
	 * You can safely store and use the pointer while the subsystem is active.
	 */
	static UCleverTapInstance* InitializeSharedInstance(
		const FCleverTapInstanceConfig& Config, const FString& CleverTapId);

	/**
	 * Tell the specific CleverTap instance to register for remote notifications.
	 */
	static void SetRemoteNotificationToken(UCleverTapInstance& Instance, const TArray<uint8>& Token);
};

}} // namespace CleverTapSDK::GenericPlatform
