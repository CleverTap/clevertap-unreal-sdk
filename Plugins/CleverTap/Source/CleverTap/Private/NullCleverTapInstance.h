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

	void PushEvent(const FString& EventName) const override;
	void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) const override;
	void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) const override;

	void DecrementValue(const FString& Key, int Amount) const override;
	void DecrementValue(const FString& Key, double Amount) const override;

	void IncrementValue(const FString& Key, int Amount) const override;
	void IncrementValue(const FString& Key, double Amount) const override;

	bool IsPushPermissionGranted() const override;
	void PromptForPushPermission(bool ShowFallbackSettings) override;
};