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
	 * The platform SDK log level to use
	 */
	ECleverTapLogLevel LogLevel{ ECleverTapLogLevel::Info };

	/**
	 * Create a FCleverTapInstanceConfig from the UObject based UCleverTapConfig.
	 */
	static FCleverTapInstanceConfig FromCleverTapConfig(const UCleverTapConfig* Config);
};

