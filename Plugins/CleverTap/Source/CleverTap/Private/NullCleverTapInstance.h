// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapInstance.h"
#include "NullCleverTapInstance.generated.h"

/**
 * A CleverTap instance implementation that does nothing and returns default values.
 */
UCLASS(NotBlueprintable)
class CLEVERTAP_API UNullCleverTapInstance : public UCleverTapInstance
{
	GENERATED_BODY()

public:
	static UNullCleverTapInstance* Create();

	FString GetCleverTapId() override;

	void OnUserLogin(const FCleverTapProperties& Profile) override;
	void OnUserLoginWithCleverTapId(const FCleverTapProperties& Profile, const FString& CleverTapId) override;

	void PushProfile(const FCleverTapProperties& Profile) override;

	void PushEvent(const FString& EventName) override;
	void PushEventWithProperties(const FString& EventName, const FCleverTapProperties& Actions) override;
	void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) override;

	TOptional<FCleverTapPropertyValue> GetProperty(const FString& Key) override;

	void DecrementIntValue(const FString& Key, int Amount) override;
	void DecrementFloatValue(const FString& Key, float Amount) override;

	void IncrementIntValue(const FString& Key, int Amount) override;
	void IncrementFloatValue(const FString& Key, float Amount) override;

	void AddMultiValueForKey(const FString& Key, const FString& Value) override;
	void AddMultiValuesForKey(const FString& Key, const TArray<FString>& Values) override;
	void RemoveMultiValueForKey(const FString& Key, const FString& Value) override;
	void RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values) override;
	void RemoveValueForKey(const FString& Key) override;
	void SetMultiValuesForKey(const FString& Key, const TArray<FString> Values) override;

	ECleverTapPushPermissionStatus GetPushPermissionStatus() override;
	void PromptForPushPermission(bool bFallbackToSettings) override;
	void PromptForPushPermissionWithAlertPrimer(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) override;
	void PromptForPushPermissionWithHalfInterstitialPrimer(
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

	void Unmute() override;
	void RecordDisplayUnitClickedEventForID(const FString& UnitID) override;
	void SetOffline(bool bIsOffline) override;
	void SetOptOut(bool bIsOptingOut) override;
	void SetNetworkInformationRecording(bool bEnableCollection) override;
	
	void DefineStringVariable(const FString& Name, const FString& DefaultValue) override;
	void DefineIntVariable(const FString& Name, int32 DefaultValue) override;
	void DefineInt64Variable(const FString& Name, int64 DefaultValue) override;
	void DefineFloatVariable(const FString& Name, float DefaultValue) override;
	void DefineDoubleVariable(const FString& Name, double DefaultValue) override;
	void DefineBoolVariable(const FString& Name, bool DefaultValue) override;
	void DefineStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue) override;
	void DefineFileVariable(const FString& Name) override;
	void FetchVariables() override;
	void SyncVariables() override;
	FString GetStringVariable(const FString& Name, const FString& DefaultValue) const override;
	int32 GetIntVariable(const FString& Name, int32 DefaultValue) const override;
	int64 GetInt64Variable(const FString& Name, int64 DefaultValue) const override;
	float GetFloatVariable(const FString& Name, float DefaultValue) const override;
	double GetDoubleVariable(const FString& Name, double DefaultValue) const override;
	bool GetBoolVariable(const FString& Name, bool DefaultValue) const override;
	TMap<FString, FString> GetStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue) const override;
	FString GetFileVariablePath(const FString& Name) const override;
	TArray<TMap<FString, FString>> GetVariants() override;
};
