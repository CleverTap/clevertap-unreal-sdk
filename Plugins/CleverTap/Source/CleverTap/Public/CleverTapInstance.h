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

	virtual void OnUserLogin(const FCleverTapProperties& Profile) const = 0;
	virtual void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) const = 0;
	virtual void PushProfile(const FCleverTapProperties& Profile) const = 0;

	virtual void PushEvent(const FString& EventName) const = 0;
	virtual void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) const = 0;
	virtual void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) const = 0;

	virtual void DecrementValue(const FString& Key, int Amount ) const = 0;
	virtual void DecrementValue(const FString& Key, double Amount ) const = 0;

	virtual void IncrementValue(const FString& Key, int Amount ) const = 0;
	virtual void IncrementValue(const FString& Key, double Amount ) const = 0;
	
};
