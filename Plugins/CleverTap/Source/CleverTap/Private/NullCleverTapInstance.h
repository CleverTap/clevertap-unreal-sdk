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

	TOptional<FCleverTapPropertyValue> GetProperty(const FString& Key) override;

	void DecrementValue(const FString& Key, int Amount) override;
	void DecrementValue(const FString& Key, double Amount) override;

	void IncrementValue(const FString& Key, int Amount) override;
	void IncrementValue(const FString& Key, double Amount) override;

	void AddMultiValueForKey(const FString& Key, const FString& Value) override;
	void AddMultiValuesForKey(const FString& Key, const TArray<FString> Values) override;
	void RemoveMultiValueForKey(const FString& Key, const FString& Value) override;
	void RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values) override;
	void RemoveValueForKey(const FString& Key) override;
	void SetMultiValuesForKey(const FString& Key, const TArray<FString> Values) override;

	ECleverTapPushPermissionStatus GetPushPermissionStatus() override;
	void PromptForPushPermission(bool bFallbackToSettings) override;
	void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) override;
	void PromptForPushPermission(
		const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig) override;

	void EnableOnPushNotificationClicked() override;
	void EnableOnOpenUrl() override;

	bool LocalizeAndroidNotificationChannel(
		const FString& ChannelID, const FText& ChannelName, const FText& ChannelDescription) override;

	bool LocalizeAndroidNotificationChannelGroup(const FString& GroupID, const FText& GroupName) override;

	void RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler) override;

	void RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter) override;

	void DiscardInAppNotifications() override;
	void ResumeInAppNotifications() override;
	void SuspendInAppNotifications() override;

	void SetOffline(bool bIsOffline) override;
	void SetOptOut(bool bIsOptingOut) override;
	void SetNetworkInformationRecording(bool bEnableCollection) override;
};
