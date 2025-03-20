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

	void OnUserLogin(const FCleverTapProfile& profile) const override;
	void OnUserLogin(const FCleverTapProfile& profile, const FString& cleverTapId) const override;

	void PushProfile(const FCleverTapProfile& profile) const override;
};
