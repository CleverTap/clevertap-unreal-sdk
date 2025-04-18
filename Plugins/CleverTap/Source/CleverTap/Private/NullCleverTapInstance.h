// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapInstance.h"

/**
 * A CleverTap instance implementation that does nothing and returns default values.
 */
class FNullCleverTapInstance : public ICleverTapInstance
{
public:
	FString GetCleverTapId() override;

	void OnUserLogin(const FCleverTapProperties& Profile) override;
	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) override;

	void PushProfile(const FCleverTapProperties& Profile) override;

	void PushEvent(const FString& EventName) override;
	void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) override;
	void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) override;

	void DecrementValue(const FString& Key, int Amount) override;
	void DecrementValue(const FString& Key, double Amount) override;

	void IncrementValue(const FString& Key, int Amount) override;
	void IncrementValue(const FString& Key, double Amount) override;

	bool IsPushPermissionGranted() override;
	void PromptForPushPermission(bool bFallbackToSettings) override;
	void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) override;
	void PromptForPushPermission(
		const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig) override;

	void EnableOnPushNotificationClicked() override;
};
