// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "CleverTapLogLevel.h"
#include "CleverTapConfig.generated.h"

/**
 * Configuration for CleverTap instances
 */
UCLASS(BlueprintType, config=Engine)
class CLEVERTAP_API UCleverTapConfig : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Gets the platform log level based on if this is a development build or not.
	 */
	ECleverTapLogLevel GetActiveLogLevel() const;

	/**
	 * If true then the UCleverTapSubsystem will setup the shared CleverTap instance when the
	 *  subsystem initializes. This value is only used on the UCleverTapConfig default object.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	bool bAutoInitializeSharedInstance = true;

	/**
	 * The project ID taken from the CleverTap dashboard
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString ProjectId;

	/**
	 * The project token taken from the CleverTap dashboard
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString ProjectToken;

	/**
	 * The region code for the project taken from https://developer.clevertap.com/docs/idc#ios
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString RegionCode;

	/**
	 * The platform SDK log level to use for development builds
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	ECleverTapLogLevel DevelopmentLogLevel = ECleverTapLogLevel::Verbose;

	/**
	 * The platform SDK log level to use for shipping builds
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	ECleverTapLogLevel ShippingLogLevel = ECleverTapLogLevel::Off;
};

