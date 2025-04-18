// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "CleverTapLogLevel.h"
#include "CleverTapConfig.generated.h"

/**
 * Configuration for CleverTap instances
 */
UCLASS(BlueprintType, config = Engine)
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
	 * Comma seperated list of user profile fields used to uniquely identify the user.
	 *
	 * Corresponds to CLEVERTAP_IDENTIFIER in the Android manifest,
	 * CleverTapIdentifiers in the iOS plist.
	 *
	 * See CleverTapInstanceConfig setIdentityKeys().
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString IdentityKeys = TEXT("Identity,Email,Phone");

	/**
	 * Must be set true in the .ini file if you will be providing a custom CleverTapId at runtime
	 * via the OnUserLogin(Profile,CleverTapId) method.
	 *
	 * Leave false to use the automatically generated CleverTapId.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	bool bUseCustomCleverTapId = false;

	/** Request internet permissions in the application manifest.
	 *  Required for CleverTap to work.
	 *  Only set this to false if you are requesting the platform-specific permissions elsewhere.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	bool bRequestInternetPermissions = true;

	/** Request the permissions needed for the GeoFence module.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	bool bRequestGeoFencePermissions = false;

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

	/**
	 * Android Only: When true, automatically integrate Google Firebase Messaging.
	 * Requires a valid AndroidGoogleServicesJsonPath.
	 *
	 * When false, Push Messaging will not function unless Firebase is
	 * being configured into the Android build elsewhere in the build process.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	bool bAndroidIntegrateFirebase = true;

	/**
	 * Android Only: Project relative path to the google-services.json
	 * file needed to configure Google Firebase Messaging / FCM.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString AndroidGoogleServicesJsonPath = TEXT("Config/google-services.json");
};
