// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"

#include "CoreMinimal.h"

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
	virtual FString GetCleverTapId() const = 0;

	/**
	 * Called to enrich an anonymous user profile with identifying information about the user. CleverTap provides
	 *  pre-defined profile properties such as name, phone, gender, age, and so on to represent well-known properties to
	 *  associate with the profile. A list of all pre-defined property names is available in the online documentation.
	 *  This overload of OnUserLogin() is only valid if bUseCustomCleverTapId == false in the configuration INI at build
	 *  time.
	 */
	virtual void OnUserLogin(const FCleverTapProperties& Profile) const = 0;

	/**
	 * Called to enrich an anonymous user profile with identifying information about the user and provide them a custom
	 *  CleverTap identifier. CleverTap provides pre-defined profile properties such as name, phone, gender, age, and so
	 *  on to represent well-known properties to associate with the profile. A list of all pre-defined property names is
	 *  available in the online documentation. This overload of OnUserLogin() is only valid if
	 *  bUseCustomCleverTapId == true in the configuration INI at build time.
	 */
	virtual void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) const = 0;

	/**
	 * Update a user's profile with additional properties.
	 */
	virtual void PushProfile(const FCleverTapProperties& Profile) const = 0;

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void DecrementValue(const FString& Key, int Amount) const = 0;

	/**
	 * Decrement a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void DecrementValue(const FString& Key, double Amount) const = 0;

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void IncrementValue(const FString& Key, int Amount) const = 0;

	/**
	 * Increment a user profile property by the specified amount. The property type must be an integer, float, or
	 *  double. The Amount value should be zero or greater than zero.
	 */
	virtual void IncrementValue(const FString& Key, double Amount) const = 0;

	/**
	 * Record a user event on the user's profile with the specified event name.
	 */
	virtual void PushEvent(const FString& EventName) const = 0;

	/**
	 * Record a user event on the user's profile with the specified event name and the associated key:value pair based
	 *  event properties.
	 */
	virtual void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) const = 0;

	/**
	 * Record a special user event to capture key details about transaction purchases. The charge details allows you to
	 *  capture properties of the transaction such as categories, transaction amount, transaction id, and user
	 *  information. The list of items allows you to record a list of items sold along with associated properties they
	 *  may have such as size, color, category, etc.
	 */
	virtual void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) const = 0;
};
