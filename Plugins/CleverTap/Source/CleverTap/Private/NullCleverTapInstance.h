// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapInstance.h"

/**
 * A CleverTap instance implementation that does nothing and returns default values.
 */
class FNullCleverTapInstance : public ICleverTapInstance
{
public:
	FString GetCleverTapId() const override;

	void OnUserLogin(const FCleverTapProperties& Profile) const override;
	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) const override;

	void PushProfile(const FCleverTapProperties& Profile) const override;
};
