// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"
#include "CleverTapPushPrimerConfig.h"

#include "CoreMinimal.h"

/**
 * Delegate type that broadcasts the eventual user response to PromptForPushPermission()
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPushPermissionResponse, bool bGranted);

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
	 * Asynchronously gets the push permission status. The callback receives a value of true if push notification
	 *  permission has been granted by the user.
	 */
	virtual void IsPushPermissionGrantedAsync(TFunction<void(bool)> Callback) = 0;

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
};
