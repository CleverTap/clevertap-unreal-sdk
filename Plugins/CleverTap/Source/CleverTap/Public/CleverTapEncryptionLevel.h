// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "CleverTapEncryptionLevel.generated.h"

/**
 * Log level for the platform's CleverTap SDK implementation
 */
UENUM(BlueprintType)
enum class ECleverTapEncryptionLevel : uint8
{
	// All stored data is in plaintext
	None,

	// PII data is encrypted completely
	Medium,
};
