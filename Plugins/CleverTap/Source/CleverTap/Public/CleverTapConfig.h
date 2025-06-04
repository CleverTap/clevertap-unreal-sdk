// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "CleverTapEncryptionLevel.h"
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

	/**
	 * The encryption level to use for PII
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	ECleverTapEncryptionLevel EncryptionLevel = ECleverTapEncryptionLevel::None;

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
	 * Android only: Project-relative path to a local copy of clevertap-android-sdk.aar.
	 * If set, this will override the default published SDK version used by the plugin.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString AndroidLocalCleverTapSdkAarPath;

	/**
	 * Android Only: When true, automatically integrate Google Firebase Messaging.
	 * Requires a valid AndroidGoogleServicesJsonPath.
	 *
	 * When false, Push Messaging will not function unless Firebase is
	 * being configured into the Android build elsewhere in the build process.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	bool bAndroidIntegrateFirebase = true;

	/**
	 * Android Only: Project relative path to the google-services.json
	 * file needed to configure Google Firebase Messaging / FCM.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	FString AndroidGoogleServicesJsonPath = TEXT("Config/google-services.json");

	/**
	 * Android Only: Default channel to use for push notifications that didn't specify a (valid) channel.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	FString AndroidDefaultNotificationChannel;

	/**
	 * Android Only: Project-relative path to the transparent PNG to use for the Android Small Notification Icon.
	 *
	 * The base filename must contain only lowercase letters (`a`-`z`), digits (`0`-`9`), or underscores (`_`).
	 *
	 * See https://developer.clevertap.com/docs/android-push#set-the-small-notification-icon
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	FString AndroidSmallNotificationIconPath;

	/**
	 * Android Only: Project-relative path to a directory of extra images to include in the application bundle's
	 * `res/drawable`.
	 *
	 * These are not part of Unreal's regular asset system. They are copied directly into the APK and must follow
	 * Android resource rules. Filenames must contain only lowercase letters (`a`-`z`), digits (`0`-`9`), or
	 * underscores (`_`).
	 *
	 * Used for push notifications and other assets referenced outside of Unreal, such as from the CleverTap dashboard.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	FString AndroidImagesDir;

	/**
	 * Android Only: Project-relative path to a directory of extra sounds to include in the application bundle's
	 * 'res/raw'.
	 *
	 * These are not part of Unreal's regular asset system. They are copied directly into the APK and must follow
	 * Android resource rules. Filenames must contain only lowercase letters (`a`-`z`), digits (`0`-`9`), or
	 * underscores (`_`).
	 *
	 * Used for push notifications and other assets referenced outside of Unreal.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	FString AndroidSoundsDir;

	/**
	 * Android Only: Provides the capability to reach users on devices that suppress notifications via GCM/FCM.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	bool bAndroidEnablePullNotifications = true;

	/**
	 * Deep links on both Android and iOS need to have their allowed schemes registered in the manifest or plist
	 *  file. These are added by the respective platform's UPL file and as such appear in the INI configuration
	 *  as 'DeepLinkSchemeFilterSlotN' variables where N is a one based index. Up to 4 slots can be configured
	 *  this way.
	 *
	 * Android Only: Enables automatic generation of an OpenUrlActivity and associated `<intent-filter>` blocks,
	 * routing matching URLs to the CleverTapInstance::OnOpenURL delegate.
	 *
	 * Configure up to 4 slots in your .ini file. Only the cross platform Scheme field, as described above, is
	 * required:
	 *
	 *	DeepLinkHostFilterSlot1=clevertap.com
	 *	DeepLinkPathPrefixFilterSlot1=/unreal-sample
	 *	AndroidIntentFilterSlot1_AutoVerify=True
	 *
	 * Note: Http/Https schemes require digital verification via assetlinks.json hosted on the target domain.
	 * See: https://developer.android.com/training/app-links/verify-android-applinks
	 *
	 * Disable OpenUrlActivity integration if you need full control over your app’s deep link handling, and plan to
	 * define your own activity with complex intent filters via custom UPL rules. If your custom activity forwards the
	 * intent to the GameActivity, it will still be routed to the OnOpenURL delegate.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	bool bIntegrateOpenUrl = true;

	/**
	 * First allowed deep link scheme (example: https). Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkSchemeFilterSlot1;

	/**
	 * Second allowed deep link scheme (example: https). Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkSchemeFilterSlot2;

	/**
	 * Third allowed deep link scheme (example: https). Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkSchemeFilterSlot3;

	/**
	 * Fourth allowed deep link scheme (example: https). Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkSchemeFilterSlot4;

	/**
	 * First allowed deep link host. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkHostFilterSlot1;

	/**
	 * Second allowed deep link host. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkHostFilterSlot2;

	/**
	 * Third allowed deep link host. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkHostFilterSlot3;

	/**
	 * Fourth allowed deep link host. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkHostFilterSlot4;

	/**
	 * First allowed deep link path prefix. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkPathPrefixFilterSlot1;

	/**
	 * Second allowed deep link path prefix. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkPathPrefixFilterSlot2;

	/**
	 * Third allowed deep link path prefix. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkPathPrefixFilterSlot3;

	/**
	 * Fourth allowed deep link path prefix. Make sure bIntegrateOpenUrl is set to true.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	FString DeepLinkPathPrefixFilterSlot4;

	/** AndroidIntentFilterSlots
	 *
	 * Note: These settings are currently only used by Android_UPL and do not need to exist in this struct.
	 *
	 * However, if we implement a cross-platform deep link system, iOS will need runtime access
	 * to this data in order to perform filtering logic &  At that point, these fields (with proper naming)
	 * should be added as real members of this struct:
	 *

		bool bAndroidIntentFilterSlot1_AutoVerify = true;

		bool bAndroidIntentFilterSlot2_AutoVerify = true;

		bool bAndroidIntentFilterSlot3_AutoVerify = true;

		bool bAndroidIntentFilterSlot4_AutoVerify = true;
	**/

	/**
	 * Android Only: Comma seperated list of Activities that shouldn't display InApp Notifications
	 *
	 * This is necessary to prevent in-app notifications from being briefly displayed on transitory screens
	 * like splash screens; when the splash screen ends so would the notification's display.
	 *
	 * The default setting is the standard Unreal SplashActivity; you should only need to change this if you've
	 * added additional custom activities.
	 *
	 * Explicit control is available via calls to SuspendInAppNotifications() and ResumeInAppNotifications()
	 *
	 * See https://developer.clevertap.com/docs/android-in-app-notifications#exclude-in-app-from-android-activity
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Android")
	FString AndroidSuspendInAppForActivities = TEXT("com.epicgames.ue4.SplashActivity");

	/**
	 * iOS Only: If true then show push notifications as a badge and in the notification center while the app is in the
	 *  foreground
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "IOS")
	bool bIOSPresentPushNotificationsInForeground = false;
};
