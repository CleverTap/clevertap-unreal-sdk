// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"
#include "CleverTapPushPrimerConfig.h"

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CleverTapInstance.generated.h"

/**
 * Delegate type that broadcasts URLS the application should open.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenUrl, const FString&, Url);

/**
 * Delegate type that broadcasts the eventual user response to PromptForPushPermission()
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushPermissionResponse, bool, bGranted);

/**
 * Delegate type used to broadcast notifications when the user taps on a Push Notification.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPushNotificationClicked, const FCleverTapProperties&, NotificationPayload);

/**
 * Delegate type used to broadcast in-app notifications when they are shown to the user.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInAppNotificationShown, const FCleverTapProperties&, NotificationPayload);

/**
 * Delegate type used to broadcast button presses on in-app notifications.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInAppNotificationButtonClicked, const FCleverTapProperties&, ButtonPayload);

/**
 * Delegate type used to broadcast in-app notifications when the user dismissed them.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnInAppNotificationDismissed, const FCleverTapProperties&, Extras, const FCleverTapProperties&, ActionExtras);

/**
 * Fired when variables have been fetched from the CleverTap server.
 * bSuccess is true if the fetch succeeded, false if it failed or timed out.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVariablesFetched, bool, bSuccess);

/**
 * Fired whenever one or more variable values have changed (either from a fetch or a server push).
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVariablesChanged);

/**
 * Status of if the user has granted permission to send push notifications.
 */
UENUM(BlueprintType)
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

UCLASS(Blueprintable, Abstract)
class CLEVERTAP_API UCleverTapInstance : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Gets the CleverTap Id associated with this instance.
	 * The CleverTap Id is a unique identifier assigned to the user profile.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual FString GetCleverTapId() PURE_VIRTUAL(UCleverTapInstance::GetCleverTapId, return TEXT(""););

	/**
	 * Called to enrich an anonymous user profile with identifying information about the user. CleverTap provides
	 *  pre-defined profile properties such as name, phone, gender, age, and so on to represent well-known properties to
	 *  associate with the profile. A list of all pre-defined property names is available in the online documentation.
	 *
	 * NOTE: This overload of OnUserLogin() is only valid if bUseCustomCleverTapId == false in the configuration INI at
	 * build time.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void OnUserLogin(const FCleverTapProperties& Profile) PURE_VIRTUAL(UCleverTapInstance::OnUserLogin, ;);

	/**
	 * Called to enrich an anonymous user profile with identifying information about the user and provide them a custom
	 *  CleverTap identifier. CleverTap provides pre-defined profile properties such as name, phone, gender, age, and so
	 *  on to represent well-known properties to associate with the profile. A list of all pre-defined property names is
	 *  available in the online documentation.
	 *
	 * NOTE: This overload of OnUserLogin() is only valid if bUseCustomCleverTapId == true in the configuration INI at
	 * build time.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void OnUserLoginWithCleverTapId(const FCleverTapProperties& Profile, const FString& CleverTapId)
		PURE_VIRTUAL(UCleverTapInstance::OnUserLoginWithCleverTapId, ;);
	;

	/**
	 * Called to enrich an anonymous user profile with identifying information about the user and provide them a custom
	 *  CleverTap identifier. CleverTap provides pre-defined profile properties such as name, phone, gender, age, and so
	 *  on to represent well-known properties to associate with the profile. A list of all pre-defined property names is
	 *  available in the online documentation.
	 *
	 * NOTE: This overload of OnUserLogin() is only valid if bUseCustomCleverTapId == true in the configuration INI at
	 * build time.
	 */
	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId)
	{
		OnUserLoginWithCleverTapId(Profile, CleverTapId);
	};

	/**
	 * Update a user's profile with additional properties.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void PushProfile(const FCleverTapProperties& Profile) PURE_VIRTUAL(UCleverTapInstance::PushProfile, ;);

	/**
	 * Returns the user profile property value for the given key, or an empty optional if not found.
	 *
	 * NOTE: May not reflect recent changes immediately due to asynchronous updates in the SDK.
	 *
	 * NOTE: Date related property values are returned as number of seconds since January 1, 1970, 00:00:00 GMT,
	 *       not the FCleverTapDate type used to set them.
	 */
	virtual TOptional<FCleverTapPropertyValue> GetProperty(const FString& Key)
		PURE_VIRTUAL(UCleverTapInstance::PushProfile, return {};);

	/** Returns the value of a property formatted as a String, or DefaultValue if the property does not exist. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	FString GetPropertyAsString(const FString& Key, const FString& DefaultValue);

	/** Returns the value of a Boolean property, or DefaultValue if the property is missing or not a Boolean. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool GetBoolProperty(const FString& Key, bool DefaultValue);

	/** Returns the value of a Date property, or DefaultValue if the property is missing, not a Date, or not convertible
	 *  to a date.
	 *
	 * Note: If the property holds an int32 or an int64 it will be treated as a unix timestamp and converted to
	 *       an FCleverTapDate. See HasDateCompatibleProperty().
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	FCleverTapDate GetDateProperty(const FString& Key, const FCleverTapDate& DefaultValue);

	/** Returns the value of a Double property, or DefaultValue if the property is missing or not a Double.
	 *
	 *  Note: FCleverTapPropertyValue supports 64-bit doubles, but Blueprint only supports 32-bit floats.
	 *        64-bit values must be accessed from C++.
	 */
	double GetDoubleProperty(const FString& Key, double DefaultValue);

	/** Returns the value of a Float property, or DefaultValue if the property is missing or not a Float.
	 *
	 *  Note: FCleverTapPropertyValue supports 64-bit doubles, but Blueprint only supports 32-bit floats.
	 *        64-bit values must be accessed from C++.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	float GetFloatProperty(const FString& Key, float DefaultValue);

	/** Returns the value of a 32-bit Integer property, or DefaultValue if the property is missing or not a 32-bit
	 *  Integer.
	 *
	 *  Note: FCleverTapPropertyValue supports 64-bit integers, but Blueprint does not.
	 *        64-bit values must be accessed from C++.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	int32 GetIntProperty(const FString& Key, int32 DefaultValue);

	/** Returns the value of a 32-bit Integer property, or DefaultValue if the property is missing or not a 64-bit
	 *  Integer.
	 *
	 *  Note: FCleverTapPropertyValue supports 64-bit integers, but Blueprint does not.
	 *        64-bit values must be accessed from C++.
	 */
	int64 GetInt64Property(const FString& Key, int64 DefaultValue);

	/** Returns the value of a String property, or DefaultValue if the property is missing or not a String. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	FString GetStringProperty(const FString& Key, const FString& DefaultValue);

	/** Returns the value of a String array property, or DefaultValue if the property is missing or not an array of
	 *  Strings. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	TArray<FString> GetStringArrayProperty(const FString& Key, const TArray<FString>& DefaultValue);

	/** Returns true if the user profile contains a property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasProperty(const FString& Key);

	/** Returns true if the user profile contains a Boolean property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasBoolProperty(const FString& Key);

	/** Returns true if the user profile contains a Date property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasDateProperty(const FString& Key);

	/** Returns true if the user profile contains a date or a value that can be converted to a date with the given Key.
	 *
	 *  int32 and int64 values are treated as Unix timestamps & considered date compatible.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasDateCompatibleProperty(const FString& Key);

	/** Returns true if the user profile contains a 64-bit Double property with the given Key.
	 *
	 *  Note: Double values cannot be accessed from Blueprint — C++ only.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasDoubleProperty(const FString& Key);

	/** Returns true if the user profile contains a Float property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasFloatProperty(const FString& Key);

	/** Returns true if the user profile contains a 32-bit Integer property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasIntProperty(const FString& Key);

	/** Returns true if the user profile contains a 64-bit Integer property with the given Key.
	 *
	 *  Note: 64-bit Integer values cannot be accessed from Blueprint — C++ only.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasInt64Property(const FString& Key);

	/** Returns true if the user profile contains a String property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasStringProperty(const FString& Key);

	/** Returns true if the user profile contains a String Array property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	bool HasStringArrayProperty(const FString& Key);

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	void DecrementValue(const FString& Key, int Amount) { DecrementIntValue(Key, Amount); }

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	void DecrementValue(const FString& Key, float Amount) { DecrementFloatValue(Key, Amount); }

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void DecrementIntValue(const FString& Key, int Amount)
		PURE_VIRTUAL(UCleverTapInstance::DecrementIntValue, ;);

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void DecrementFloatValue(const FString& Key, float Amount)
		PURE_VIRTUAL(UCleverTapInstance::DecrementFloatValue, ;);

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	void IncrementValue(const FString& Key, int Amount) { IncrementIntValue(Key, Amount); }

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	void IncrementValue(const FString& Key, float Amount) { IncrementFloatValue(Key, Amount); }

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void IncrementIntValue(const FString& Key, int Amount)
		PURE_VIRTUAL(UCleverTapInstance::IncrementIntValue, ;);

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void IncrementFloatValue(const FString& Key, float Amount)
		PURE_VIRTUAL(UCleverTapInstance::IncrementFloatValue, ;);

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
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void AddMultiValueForKey(const FString& Key, const FString& Value)
		PURE_VIRTUAL(UCleverTapInstance::AddMultiValueForKey, ;);

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
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void AddMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
		PURE_VIRTUAL(UCleverTapInstance::AddMultiValuesForKey, ;);

	/**
	 * Remove a unique value from a multi-value user profile property.
	 *
	 * If the key currently contains a scalar value, prior to performing the remove operation
	 * the key will be promoted to a multi-value property with the current value cast to a string.
	 * If the multi-value property is empty after the remove operation, the key will be removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void RemoveMultiValueForKey(const FString& Key, const FString& Value)
		PURE_VIRTUAL(UCleverTapInstance::RemoveMultiValueForKey, ;);

	/**
	 * Remove a collection of unique values from a multi-value user profile property
	 *
	 * If the key currently contains a scalar value, prior to performing the remove operation
	 * the key will be promoted to a multi-value property with the current value cast to a string.
	 *
	 * If the multi-value property is empty after the remove operation, the key will be removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
		PURE_VIRTUAL(UCleverTapInstance::RemoveMultiValuesForKey, ;);

	/**
	 * Remove the user profile property value specified by key from the user profile.
	 *
	 * This method can be used to remove PII data (for eg. Email,Name,Phone), locally from database and shared prefs.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void RemoveValueForKey(const FString& Key) PURE_VIRTUAL(UCleverTapInstance::RemoveValueForKey, ;);

	/**
	 * Set a collection of unique values as a multi-value user profile property.
	 *
	 * Any existing value will be overwritten.
	 *
	 * Max 100 values, on reaching 100 cap, oldest value(s) will be removed.
	 * Values must be Strings and are limited to 512 characters.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void SetMultiValuesForKey(const FString& Key, const TArray<FString> Values)
		PURE_VIRTUAL(UCleverTapInstance::SetMultiValuesForKey, ;);

	/** Copy all matching profile properties to a UObject */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	void ApplyProfileToObject(UObject* Target);

	/** Copy all matching profile properties to a UStruct-based type T */
	template <typename T>
	void ApplyProfileToStruct(T* Target);

	/**
	 * Record a user event on the user's profile with the specified event name.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void PushEvent(const FString& EventName) PURE_VIRTUAL(UCleverTapInstance::PushEvent, ;);

	/**
	 * Record a user event on the user's profile with the specified event name and the associated key:value pair based
	 *  event properties.
	 */
	void PushEvent(const FString& EventName, const FCleverTapProperties& EventProperties)
	{
		PushEventWithProperties(EventName, EventProperties);
	}

	/**
	 * Record a user event on the user's profile with the specified event name and the associated key:value pair based
	 *  event properties.
	 *
	 * Blueprint callable version of the PushEvent(String,FCleverTapProperties) overload.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void PushEventWithProperties(const FString& EventName, const FCleverTapProperties& EventProperties)
		PURE_VIRTUAL(UCleverTapInstance::PushEventWithProperties, ;);

	/**
	 * Record a special user event to capture key details about transaction purchases. The charge details allows you to
	 *  capture properties of the transaction such as categories, transaction amount, transaction id, and user
	 *  information. The list of items allows you to record a list of items sold along with associated properties they
	 *  may have such as size, color, category, etc.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Profile")
	virtual void PushChargedEvent(const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items)
		PURE_VIRTUAL(UCleverTapInstance::PushChargedEvent, ;);

	/**
	 * Gets the push permission status. If this returns ECleverTapPushPermissionStatus::Unknown then it should be polled
	 *  until the status has been determined.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Push")
	virtual ECleverTapPushPermissionStatus GetPushPermissionStatus()
		PURE_VIRTUAL(UCleverTapInstance::GetPushPermissionStatus, return ECleverTapPushPermissionStatus::Unknown;);

	/**
	 * Prompts the user to grant push permissions, if they've not already been granted.
	 *
	 *\param bFallbackToSettings - when this is true and permissions were previously denied, then show an alert dialog
	 *                             which routes to app's notification settings page.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Push")
	virtual void PromptForPushPermission(bool bFallbackToSettings)
		PURE_VIRTUAL(UCleverTapInstance::PromptForPushPermission, ;);

	/**
	 * Prompts the user to grant push permissions using a push primer alert, if they've not already been granted.
	 *
	 * A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
	 * It is an InApp notification that provides the details of message types, your users can expect, before requesting
	 * notification permission.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Push")
	virtual void PromptForPushPermissionWithAlertPrimer(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig)
		PURE_VIRTUAL(UCleverTapInstance::PromptForPushPermissionWithAlertPrimer, ;);

	/**
	 * Prompts the user to grant push permissions using a push primer alert, if they've not already been granted.
	 *
	 * A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
	 * It is an InApp notification that provides the details of message types, your users can expect, before requesting
	 * notification permission.
	 */
	void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig)
	{
		PromptForPushPermissionWithAlertPrimer(PushPrimerAlertConfig);
	}

	/**
	 * Prompts the user to grant push permissions using a push primer half-interstitial, if they've not already been
	 * granted.
	 *
	 * A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
	 * It is an InApp notification that provides the details of message types, your users can expect, before requesting
	 * notification permission.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Push")
	virtual void PromptForPushPermissionWithHalfInterstitialPrimer(
		const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig)
		PURE_VIRTUAL(UCleverTapInstance::PromptForPushPermissionWithHalfInterstitialPrimer, ;);

	/**
	 * Prompts the user to grant push permissions using a push primer half-interstitial, if they've not already been
	 * granted or denied.
	 *
	 * A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
	 * It is an InApp notification that provides the details of message types, your users can expect, before requesting
	 * notification permission.
	 */
	void PromptForPushPermission(const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig)
	{
		PromptForPushPermissionWithHalfInterstitialPrimer(PushPrimerHalfInterstitialConfig);
	}

	/**
	 * Delegate that broadcasts the eventual user response to PromptForPushPermission()
	 */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|Push")
	FOnPushPermissionResponse OnPushPermissionResponse;

	/**
	 * Called when the user taps a push notification.
	 *
	 * Initially paused; call EnableOnPushNotificationClicked() once event handlers are connected and game systems are
	 * ready.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|Push")
	FOnPushNotificationClicked OnPushNotificationClicked;

	/**
	 * Enables delivery of push notification click events.
	 *
	 * Call this after binding to OnPushNotificationClicked and your systems are ready to handle incoming events.
	 * If the game was launched by clicking on a push notification, it will be delivered immediately after this
	 * call. If multiple notifications are received before this call, only the most recent will be delivered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Push")
	virtual void EnableOnPushNotificationClicked() PURE_VIRTUAL(UCleverTapInstance::EnableOnPushNotificationClicked, ;);

	/**
	 * Called when the application should open a URL (e.g. from clicking a deep link in a notification).
	 *
	 * Initially paused; call EnableOnOpenUrl() once event handlers are connected and game systems are
	 * ready.
	 *
	 * For the operating system to route URLs to your application the schemes need to be
	 *          registered as intent filters on Android or in the plist for iOS.
	 *          See `bIntegrateOpenUrl`
	 */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|System")
	FOnOpenUrl OnOpenUrl;

	/**
	 * Enables delivery of OnOpenUrl events.
	 *
	 * Call this after binding to OnOpenUrl and your systems are ready to handle incoming events.
	 * If the game was launched by clicking on a registerd URL scheme, OnOpenUrl will be delivered immediately after
	 * this call. If multiple notifications are received before this call, only the most recent will be delivered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|System")
	virtual void EnableOnOpenUrl() PURE_VIRTUAL(UCleverTapInstance::EnableOnOpenUrl, ;);

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
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Push")
	virtual bool LocalizeAndroidNotificationChannel(
		const FString& ChannelID, const FText& ChannelName, const FText& ChannelDescription)
		PURE_VIRTUAL(UCleverTapInstance::LocalizeAndroidNotificationChannel, return false;);

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
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Push")
	virtual bool LocalizeAndroidNotificationChannelGroup(const FString& GroupID, const FText& GroupName)
		PURE_VIRTUAL(UCleverTapInstance::LocalizeAndroidNotificationChannel, return false;);

	/**
	 * iOS Only: Register a URL handler if you would like to implement custom handling for URLs in the case of in-app
	 *  notification CTAs and push notifications.
	 *
	 * The handler is unique and registering a second handler replaces the original. The handler should return true if
	 *  if you would like CleverTap to open the URL supplied to it. There is no guarantee on the thread that the
	 *  handler is called on.
	 */
	virtual void RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler)
		PURE_VIRTUAL(UCleverTapInstance::LocalizeAndroidNotificationChannel, ;);

	/**
	 * Called when an in-app notification is shown to the user.
	 *
	 * Note: InApp notifications are initially suspended; call ResumeInAppNotifications() when your delegates are
	 * connected and your application is prepared to handle the notifications.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|InApp")
	FOnInAppNotificationShown OnInAppNotificationShown;

	/**
	 * Called when an in-app notification is dismissed by the user.
	 *
	 * Note: InApp notifications are initially suspended; call ResumeInAppNotifications() when your delegates are
	 * connected and your application is prepared to handle the notifications.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|InApp")
	FOnInAppNotificationDismissed OnInAppNotificationDismissed;

	/**
	 * Called when the user clicks on a Key/Value pair button in an in-app notification.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|InApp")
	FOnInAppNotificationButtonClicked OnInAppNotificationButtonClicked;

	/**
	 * Called before an in-app notification is shown to the user to determine if it should actually be shown. The
	 *  first parameter of this filter is the key/value pairs of the notification set from the CleverTap dashboard.
	 *  The filter should return true if the in-app notification should be shown and false when it should be
	 *  surppressed. Note that there is no guarantee what thread the filter function is invoked on.
	 */
	virtual void RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter)
		PURE_VIRTUAL(UCleverTapInstance::RegisterInAppNotificationFilter, ;);

	/**
	 * Disables the display of InApp notifications and discards any new incoming.
	 *
	 * The InApp Notifications will be displayed again only once ResumeInAppNotifications() is called.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|InApp")
	virtual void DiscardInAppNotifications() PURE_VIRTUAL(UCleverTapInstance::DiscardInAppNotifications, ;);
	;

	/** Resumes displaying in-app notifications.
	 *
	 *  Any notifacations queued by SuspendInAppNotifications() will be instantly shown.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|InApp")
	virtual void ResumeInAppNotifications() PURE_VIRTUAL(UCleverTapInstance::ResumeInAppNotifications, ;);

	/** Suspends and saves in-app notifications until ResumeInAppNotifications() is called.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|InApp")
	virtual void SuspendInAppNotifications() PURE_VIRTUAL(UCleverTapInstance::SuspendInAppNotifications, ;);

	/** Disables or enables sending events to the server.
	 *
	 * To stop recorded events from being sent to the server, use this method to set the SDK instance to
	 * offline. Once offline, events will be recorded and queued locally but will not be sent to the server until
	 * offline is disabled.
	 *
	 * Calling this method again with bIsOffline set to false will allow events to be sent to server and
	 * the SDK instance will immediately attempt to send events that have been queued while offline.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|System")
	virtual void SetOffline(bool bIsOffline) PURE_VIRTUAL(UCleverTapInstance::SetOffline, ;);

	/**
	 * Can be used to stop sending events to CleverTap for GDPR compliance. Calling this method with bIsOptingOut
	 *  set to false will resume sending events to CleverTap. This value is not remembered across app sessions so
	 *  it is best practice to call this method with the correct state as early as possible after initialization.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|System")
	virtual void SetOptOut(bool bIsOptingOut) PURE_VIRTUAL(UCleverTapInstance::SetOptOut, ;);

	/**
	 * CleverTap does not track network information by default for GDPR compliance. Enabling collection will
	 *  collection personal information like Wifi, network information, and user IP.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|System")
	virtual void SetNetworkInformationRecording(bool bEnableCollection)
		PURE_VIRTUAL(UCleverTapInstance::SetNetworkInformationRecording, ;);
	
	
	// -------------------------------------------------------------------------
	// Product Experiences (Variables)
	// -------------------------------------------------------------------------

	/**
	 * Define a string variable. Call this at startup before FetchVariables().
	 * If the variable already exists the default value is ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual void DefineStringVariable(const FString& Name, const FString& DefaultValue)
		PURE_VIRTUAL(UCleverTapInstance::DefineStringVariable, ;);

	/** Define a 32-bit integer variable (equivalent to Unity's int overload). */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual void DefineIntVariable(const FString& Name, int32 DefaultValue)
		PURE_VIRTUAL(UCleverTapInstance::DefineIntVariable, ;);

	/** Define a 64-bit integer variable (equivalent to Unity's long overload). */
	virtual void DefineInt64Variable(const FString& Name, int64 DefaultValue)
		PURE_VIRTUAL(UCleverTapInstance::DefineInt64Variable, ;);

	/**
	 * Convenience: define a short (16-bit integer) variable.
	 * Internally stored as a 32-bit integer; clamped to [-32768, 32767].
	 * Clamping is enforced unconditionally at this layer and cannot be overridden.
	 */
	void DefineShortVariable(const FString& Name, int32 DefaultValue)
	{
		DefineIntVariable(Name, FMath::Clamp(DefaultValue, -32768, 32767));
	}

	/**
	 * Convenience: define a byte (8-bit unsigned integer) variable.
	 * Internally stored as a 32-bit integer; clamped to [0, 255].
	 * Clamping is enforced unconditionally at this layer and cannot be overridden.
	 */
	void DefineByteVariable(const FString& Name, int32 DefaultValue)
	{
		DefineIntVariable(Name, FMath::Clamp(DefaultValue, 0, 255));
	}

	/** Define a single-precision float variable (equivalent to Unity's float overload). */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual void DefineFloatVariable(const FString& Name, float DefaultValue)
		PURE_VIRTUAL(UCleverTapInstance::DefineFloatVariable, ;);

	/** Define a double-precision float variable (equivalent to Unity's double overload). */
	virtual void DefineDoubleVariable(const FString& Name, double DefaultValue)
		PURE_VIRTUAL(UCleverTapInstance::DefineDoubleVariable, ;);

	/** Define a boolean variable. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual void DefineBoolVariable(const FString& Name, bool DefaultValue)
		PURE_VIRTUAL(UCleverTapInstance::DefineBoolVariable, ;);

	/**
	 * Define a string-keyed string-map variable (equivalent to Unity's Dictionary<string,string> overload).
	 * Maps to the "group" kind in the CleverTap backend.
	 */
	virtual void DefineStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue)
		PURE_VIRTUAL(UCleverTapInstance::DefineStringMapVariable, ;);

	/**
	 * Define a file variable. The value is a local file path, populated after FetchVariables()
	 * downloads the file (equivalent to Unity's FileVariable overload).
	 */
	virtual void DefineFileVariable(const FString& Name)
		PURE_VIRTUAL(UCleverTapInstance::DefineFileVariable, ;);

	/**
	 * Fetch current variable values from CleverTap servers.
	 * OnVariablesFetched fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual void FetchVariables()
		PURE_VIRTUAL(UCleverTapInstance::FetchVariables, ;);

	/**
	 * Sync variable definitions to the CleverTap dashboard (development tool).
	 * Call this during development after DefineXxxVariable calls so the dashboard
	 * can recognize the variables and let you configure their values.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual void SyncVariables()
		PURE_VIRTUAL(UCleverTapInstance::SyncVariables, ;);

	/** Get the current value of a string variable. Returns DefaultValue if not yet fetched. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual FString GetStringVariable(const FString& Name, const FString& DefaultValue) const
		PURE_VIRTUAL(UCleverTapInstance::GetStringVariable, return DefaultValue;);

	/** Get the current value of a 32-bit integer variable. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual int32 GetIntVariable(const FString& Name, int32 DefaultValue) const
		PURE_VIRTUAL(UCleverTapInstance::GetIntVariable, return DefaultValue;);

	/** Get the current value of a 64-bit integer variable. */
	virtual int64 GetInt64Variable(const FString& Name, int64 DefaultValue) const
		PURE_VIRTUAL(UCleverTapInstance::GetInt64Variable, return DefaultValue;);

	/** Get the current value of a float variable. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual float GetFloatVariable(const FString& Name, float DefaultValue) const
		PURE_VIRTUAL(UCleverTapInstance::GetFloatVariable, return DefaultValue;);

	/** Get the current value of a double variable. */
	virtual double GetDoubleVariable(const FString& Name, double DefaultValue) const
		PURE_VIRTUAL(UCleverTapInstance::GetDoubleVariable, return DefaultValue;);

	/** Get the current value of a boolean variable. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|PE")
	virtual bool GetBoolVariable(const FString& Name, bool DefaultValue) const
		PURE_VIRTUAL(UCleverTapInstance::GetBoolVariable, return DefaultValue;);

	/** Get the current value of a string-map variable. */
	virtual TMap<FString, FString> GetStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue) const
		PURE_VIRTUAL(UCleverTapInstance::GetStringMapVariable, return DefaultValue;);

	/**
	 * Get the local file path of a file variable after it has been fetched.
	 * Returns empty string if the file has not yet been downloaded.
	 */
	virtual FString GetFileVariablePath(const FString& Name) const
		PURE_VIRTUAL(UCleverTapInstance::GetFileVariablePath, return TEXT(""););

	/**
	 * Returns the active A/B test variants assigned to the current user.
	 * Each entry is a dictionary of variant properties (e.g. "name", "id").
	 * Returns an empty array if no variants are assigned.
	 * Note: TArray<TMap> is not Blueprint-serializable; call from C++ only.
	 */
	virtual TArray<TMap<FString, FString>> GetVariants()
		PURE_VIRTUAL(UCleverTapInstance::GetVariants, return {};);

	/** Delegate that fires when FetchVariables() completes */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|PE")
	FOnVariablesFetched OnVariablesFetched;

	/** Delegate that fires whenever any variable value changes */
	UPROPERTY(BlueprintAssignable, Category = "CleverTap|PE")
	FOnVariablesChanged OnVariablesChanged;

private:
	/** Copy all matching profile properties to a UStruct-based type T */
	void ApplyProfileToStruct(const UStruct* StructDef, void* TargetStructInstance);
};

template <typename T>
inline void UCleverTapInstance::ApplyProfileToStruct(T* Target)
{
	ApplyProfileToStruct(TBaseStructure<T>::Get(), Target);
}
