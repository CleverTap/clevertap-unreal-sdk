// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProfile.h"

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
};
