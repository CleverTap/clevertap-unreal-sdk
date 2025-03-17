// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "CleverTapLogLevel.generated.h"

/**
 * Log level for the platform's CleverTap SDK implementation
 */
UENUM(BlueprintType)
enum class ECleverTapLogLevel : uint8
{
	// No platform logging will be performed
	Off,

	// (Default) Minimal SDK integration related messages
	Info,

	// Logs warnings and other important information
	Debug,

	// If supported by the platform this may contain more information than debug
	Verbose,
};

