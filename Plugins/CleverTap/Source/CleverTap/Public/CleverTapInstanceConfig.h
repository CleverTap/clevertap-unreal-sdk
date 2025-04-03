// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapLogLevel.h"
#include "CoreMinimal.h"

class UCleverTapConfig;

/**
 */
struct FCleverTapInstanceConfig
{
	/**
	 * The project ID taken from the CleverTap dashboard
	 */
	FString ProjectId;

	/**
	 * The project token taken from the CleverTap dashboard
	 */
	FString ProjectToken;

	/**
	 * The region code for the project taken from https://developer.clevertap.com/docs/idc#ios
	 */
	FString RegionCode;

	/**
	 * Comma seperated list of user profile fields used to uniquely identify the user.
	 *
	 * Corresponds to CLEVERTAP_IDENTIFIER in the Android manifest,
	 * CleverTapIdentifiers in the iOS plist.
	 *
	 * See CleverTapInstanceConfig setIdentityKeys().
	 */
	FString IdentityKeys;

	/**
	 * Returns the IdentityKeys field as an array.
	 */
	TArray<FString> GetIdentityKeys() const;

	/**
	 * The platform SDK log level to use
	 */
	ECleverTapLogLevel LogLevel{ ECleverTapLogLevel::Info };

	/**
	 * Create a FCleverTapInstanceConfig from the UObject based UCleverTapConfig.
	 */
	static FCleverTapInstanceConfig FromCleverTapConfig(const UCleverTapConfig* Config);
};
