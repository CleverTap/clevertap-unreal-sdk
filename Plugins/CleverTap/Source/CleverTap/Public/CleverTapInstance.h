// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"
#include "CleverTapPushPrimerConfig.h"

#include "CoreMinimal.h"

/**
 * Delegate type that broadcasts URLS the application should open.
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpenUrl, const FString& Url);

/**
 * Delegate type that broadcasts the eventual user response to PromptForPushPermission()
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPushPermissionResponse, bool bGranted);

/**
 * Delegate type used to broadcast notifications when the user taps on a Push Notification.
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPushNotificationClicked, const FCleverTapProperties& NotificationPayload);

/**
 * Delegate type used to broadcast in-app notifications when they are shown to the user.
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInAppNotificationShown, const FCleverTapProperties& NotificationPayload);

/**
 * Delegate type used to broadcast in-app notifications when the user dismissed them.
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnInAppNotificationDismissed, const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras);

/**
 * Status of if the user has granted permission to send push notifications.
 */
enum class ECleverTapPushPermissionStatus : uint8
{
	// The permission status is currently unknown
	Unknown,

	// Permission has been granted
	Granted,

	// Permission has not been granted
	NotGranted,
};

/**
 * A CleverTap channel that a link or notification came from
 */
enum class ECleverTapChannel : uint8
{
	PushNotification,
	AppInbox,
	InAppNotification,
};

/**
 * A CleverTap API instance
 */
class CLEVERTAP_API ICleverTapInstance
{
public:
	virtual ~ICleverTapInstance() = default;

	/**
	 * Gets the CleverTap Id associated with this instance. The CleverTap Id is a unique identifier
	 *  assigned to the user profile.
	 */
	virtual FString GetCleverTapId() = 0;

	/**
	 * Called to enrich an anonymous user profile with identifying information about the user. CleverTap provides
	 *  pre-defined profile properties such as name, phone, gender, age, and so on to represent well-known properties to
	 *  associate with the profile. A list of all pre-defined property names is available in the online documentation.
	 *  This overload of OnUserLogin() is only valid if bUseCustomCleverTapId == false in the configuration INI at build
	 *  time.
	 */
	virtual void OnUserLogin(const FCleverTapProperties& Profile) = 0;

	/**
	 * Called to enrich an anonymous user profile with identifying information about the user and provide them a custom
	 *  CleverTap identifier. CleverTap provides pre-defined profile properties such as name, phone, gender, age, and so
	 *  on to represent well-known properties to associate with the profile. A list of all pre-defined property names is
	 *  available in the online documentation. This overload of OnUserLogin() is only valid if
	 *  bUseCustomCleverTapId == true in the configuration INI at build time.
	 */
	virtual void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) = 0;

	/**
	 * Update a user's profile with additional properties.
	 */
	virtual void PushProfile(const FCleverTapProperties& Profile) = 0;

	/**
	 * Returns the user profile property value for the given key, or an empty optional if not found.
	 *
	 * NOTE: May not reflect recent changes immediately due to asynchronous updates in the SDK.
	 *
	 * NOTE: Date related property values are returned as number of seconds since January 1, 1970, 00:00:00 GMT,
	 *       not the FCleverTapDate type used to set them.
	 */
	virtual TOptional<FCleverTapPropertyValue> GetProperty(const FString& Key) = 0;

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void DecrementValue(const FString& Key, int Amount) = 0;

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void DecrementValue(const FString& Key, double Amount) = 0;

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void IncrementValue(const FString& Key, int Amount) = 0;

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void IncrementValue(const FString& Key, double Amount) = 0;

	/**
	 * Add a unique value to a multi-value user profile property
	 * If the property does not exist it will be created.
	 *
	 * Max 100 values, on reaching 100 cap, oldest value(s) will be removed.
	 * Values must be Strings and are limited to 512 characters.
	 *
	 * If the key currently contains a scalar value, the key will be promoted to a multi-value property
	 * with the current value cast to a string and the new value(s) added
	 */
	virtual void AddMultiValueForKey(const FString& Key, const FString& Value) = 0;

	/**
	 * Add a collection of unique values to a multi-value user profile property
	 * If the property does not exist it will be created
	 *
	 * Max 100 values, on reaching 100 cap, oldest value(s) will be removed.
	 * Values must be Strings and are limited to 512 characters.
	 *
	 * If the key currently contains a scalar value, the key will be promoted to a multi-value property
	 * with the current value cast to a string and the new value(s) added
	 */
	virtual void AddMultiValuesForKey(const FString& Key, const TArray<FString> Values) = 0;

	/**
	 * Remove a unique value from a multi-value user profile property.
	 *
	 * If the key currently contains a scalar value, prior to performing the remove operation
	 * the key will be promoted to a multi-value property with the current value cast to a string.
	 * If the multi-value property is empty after the remove operation, the key will be removed.
	 */
	virtual void RemoveMultiValueForKey(const FString& Key, const FString& Value) = 0;

	/**
	 * Remove a collection of unique values from a multi-value user profile property
	 *
	 * If the key currently contains a scalar value, prior to performing the remove operation
	 * the key will be promoted to a multi-value property with the current value cast to a string.
	 *
	 * If the multi-value property is empty after the remove operation, the key will be removed.
	 */
	virtual void RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values) = 0;

	/**
	 * Remove the user profile property value specified by key from the user profile.
	 *
	 * This method can be used to remove PII data (for eg. Email,Name,Phone), locally from database and shared prefs.
	 */
	virtual void RemoveValueForKey(const FString& Key) = 0;

	/**
	 * Set a collection of unique values as a multi-value user profile property.
	 *
	 * Any existing value will be overwritten.
	 *
	 * Max 100 values, on reaching 100 cap, oldest value(s) will be removed.
	 * Values must be Strings and are limited to 512 characters.
	 */
	virtual void SetMultiValuesForKey(const FString& Key, const TArray<FString> Values) = 0;

	/**
	 * Record a user event on the user's profile with the specified event name.
	 */
	virtual void PushEvent(const FString& EventName) = 0;

	/**
	 * Record a user event on the user's profile with the specified event name and the associated key:value pair based
	 *  event properties.
	 */
	virtual void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) = 0;

	/**
	 * Record a special user event to capture key details about transaction purchases. The charge details allows you to
	 *  capture properties of the transaction such as categories, transaction amount, transaction id, and user
	 *  information. The list of items allows you to record a list of items sold along with associated properties they
	 *  may have such as size, color, category, etc.
	 */
	virtual void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) = 0;

	/**
	 * Gets the push permission status. If this returns ECleverTapPushPermissionStatus::Unknown then it should be polled
	 *  until the status has been determined.
	 */
	virtual ECleverTapPushPermissionStatus GetPushPermissionStatus() = 0;

	/**
	 * Prompts the user to grant push permissions, if they've not already been granted or denied.
	 *
	 *\param bFallbackToSettings - when this is true and permissions were previously denied, then show an alert dialog
	 *                             which routes to app's notification settings page.
	 */
	virtual void PromptForPushPermission(bool bFallbackToSettings) = 0;

	/**
	 * Prompts the user to grant push permissions using a push primer alert, if they've not already been granted or
	 * denied.
	 *
	 * A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
	 * It is an InApp notification that provides the details of message types, your users can expect, before requesting
	 * notification permission.
	 */
	virtual void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) = 0;

	/**
	 * Prompts the user to grant push permissions using a push primer half-interstitial, if they've not already been
	 * granted or denied.
	 *
	 * A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
	 * It is an InApp notification that provides the details of message types, your users can expect, before requesting
	 * notification permission.
	 */
	virtual void PromptForPushPermission(
		const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig) = 0;

	/**
	 * Delegate that broadcasts the eventual user response to PromptForPushPermission()
	 */
	FOnPushPermissionResponse OnPushPermissionResponse;

	/**
	 * Called when the user taps a push notification.
	 *
	 * Initially paused; call EnableOnPushNotificationClicked() once event handlers are connected and game systems are
	 * ready.
	 */
	FOnPushNotificationClicked OnPushNotificationClicked;

	/**
	 * Called when the application should open a URL (e.g. from clicking a deep link in a notification).
	 *
	 * Initially paused; call EnableOnPushNotificationClicked() once event handlers are connected and game systems are
	 * ready.
	 *
	 * Android: For the operating system to route URLs to your application the schemes need to be
	 *          registered as intent filters in the application manifest.
	 *          See `bAndroidIntegrateOpenUrlActivity`
	 */
	FOnOpenUrl OnOpenUrl;

	/**
	 * Enables delivery of push notification click events.
	 *
	 * Call this after binding to OnPushNotificationClicked and your systems are ready to handle incoming events.
	 * If the game was launched by clicking on a push notification, it will be delivered immediately after this
	 * call. If multiple notifications are received before this call, only the most recent will be delivered.
	 */
	virtual void EnableOnPushNotificationClicked() = 0;

	/**
	 * Android Only: Updates the name and description of an existing Notification Channel with localized text.
	 *
	 * Returns true on success. Returns false if a channel with this ID does not exist, or the underlying OS doesn't
	 * support notification channels. No-op on non-Android platforms.
	 *
	 * Notification Channels must must be preconfigured in your project’s `Config/DefaultEngine.ini`;
	 * this allows them to be registered during the Java `GameApplication.onCreate()` method before Unreal has
	 * initialized.
	 *
	 * Call this function during startup (and at locale change) to update the name and description strings.
	 */
	virtual bool LocalizeAndroidNotificationChannel(
		const FString& ChannelID, const FText& ChannelName, const FText& ChannelDescription) = 0;

	/**
	 * Android Only: Updates the name of an existing Notification Channel Group with localized text.
	 *
	 * Returns true on success. Returns false if a group with this ID does not exist, or the underlying OS doesn't
	 * support notification channels. No-op on non-Android platforms.
	 *
	 * Notification Channel Groups must must be preconfigured in your project’s `Config/DefaultEngine.ini`;
	 * this allows them to be registered during the Java `GameApplication.onCreate()` method before Unreal has
	 * initialized.
	 *
	 * Call this function during startup (and at locale change) to update with the localized name.
	 */
	virtual bool LocalizeAndroidNotificationChannelGroup(const FString& GroupID, const FText& GroupName) = 0;

	/**
	 * iOS Only: Register a URL handler if you would like to implement custom handling for URLs in the case of in-app
	 *  notification CTAs and push notifications.
	 *
	 * The handler is unique and registering a second handler replaces the original. The handler should return true if
	 *  if you would like CleverTap to open the URL supplied to it. There is no guarantee on the thread that the
	 *  handler is called on.
	 */
	virtual void RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler) = 0;

	/**
	 * Called when an in-app notification is shown to the user.
	 *
	 * Note: InApp notifications are initially suspended; call ResumeInAppNotifications() when your delegates are
	 * connected and your application is prepared to handle the notifications.
	 */
	FOnInAppNotificationShown OnInAppNotificationShown;

	/**
	 * Called when an in-app notification is dismissed by the user.
	 *
	 * Note: InApp notifications are initially suspended; call ResumeInAppNotifications() when your delegates are
	 * connected and your application is prepared to handle the notifications.
	 */
	FOnInAppNotificationDismissed OnInAppNotificationDismissed;

	/**
	 * Called before an in-app notification is shown to the user to determine if it should actually be shown. The
	 *  first parameter of this filter is the key/value pairs of the notification set from the CleverTap dashboard.
	 *  The filter should return true if the in-app notification should be shown and false when it should be
	 *  surppressed. Note that there is no guarantee what thread the filter function is invoked on.
	 */
	virtual void RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter) = 0;

	/**
	 * Disables the display of InApp notifications and discards any new incoming.
	 *
	 * The InApp Notifications will be displayed again only once ResumeInAppNotifications() is called.
	 */
	virtual void DiscardInAppNotifications() = 0;

	/** Resumes displaying in-app notifications.
	 *
	 *  Any notifacations queued by SuspendInAppNotifications() will be instantly shown.
	 */
	virtual void ResumeInAppNotifications() = 0;

	/** Suspends and saves in-app notifications until ResumeInAppNotifications() is called.
	 */
	virtual void SuspendInAppNotifications() = 0;

	/** Disables or enables sending events to the server.
	 *
	 * To stop recorded events from being sent to the server, use this method to set the SDK instance to
	 * offline. Once offline, events will be recorded and queued locally but will not be sent to the server until
	 * offline is disabled.
	 *
	 * Calling this method again with bIsOffline set to false will allow events to be sent to server and
	 * the SDK instance will immediately attempt to send events that have been queued while offline.
	 */
	virtual void SetOffline(bool bIsOffline) = 0;

	/**
	 * Can be used to stop sending events to CleverTap for GDPR compliance. Calling this method with bIsOptingOut
	 *  set to false will resume sending events to CleverTap. This value is not remembered across app sessions so
	 *  it is best practice to call this method with the correct state as early as possible after initialization.
	 */
	virtual void SetOptOut(bool bIsOptingOut) = 0;

	/**
	 * CleverTap does not track network information by default for GDPR compliance. Enabling collection will
	 *  collection personal information like Wifi, network information, and user IP.
	 */
	virtual void SetNetworkInformationRecording(bool bEnableCollection) = 0;
};
