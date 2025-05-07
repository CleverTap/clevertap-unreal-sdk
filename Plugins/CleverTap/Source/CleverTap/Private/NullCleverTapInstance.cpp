// Copyright CleverTap All Rights Reserved.
#include "NullCleverTapInstance.h"

#include "CleverTapUtilities.h"

FString FNullCleverTapInstance::GetCleverTapId()
{
	return FString{};
}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile)
{
	CleverTapSDK::Ignore(Profile);
}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId)
{
	CleverTapSDK::Ignore(Profile, CleverTapId);
}

void FNullCleverTapInstance::PushProfile(const FCleverTapProperties& Profile)
{
	CleverTapSDK::Ignore(Profile);
}

void FNullCleverTapInstance::PushEvent(const FString& EventName)
{
	CleverTapSDK::Ignore(EventName);
}

void FNullCleverTapInstance::PushEvent(const FString& EventName, const FCleverTapProperties& Actions)
{
	CleverTapSDK::Ignore(EventName, Actions);
}

void FNullCleverTapInstance::PushChargedEvent(
	const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items)
{
	CleverTapSDK::Ignore(ChargeDetails, Items);
}

TOptional<FCleverTapPropertyValue> FNullCleverTapInstance::GetProperty(const FString& Key)
{
	CleverTapSDK::Ignore(Key);
	return {};
}

void FNullCleverTapInstance::DecrementValue(const FString& Key, int Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void FNullCleverTapInstance::DecrementValue(const FString& Key, double Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void FNullCleverTapInstance::IncrementValue(const FString& Key, int Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void FNullCleverTapInstance::IncrementValue(const FString& Key, double Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void FNullCleverTapInstance::AddMultiValueForKey(const FString& Key, const FString& Value)
{
	CleverTapSDK::Ignore(Key, Value);
}

void FNullCleverTapInstance::AddMultiValuesForKey(const FString& Key, const TArray<FString> Values)
{
	CleverTapSDK::Ignore(Key, Values);
}

void FNullCleverTapInstance::RemoveMultiValueForKey(const FString& Key, const FString& Value)
{
	CleverTapSDK::Ignore(Key, Value);
}

void FNullCleverTapInstance::RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
{
	CleverTapSDK::Ignore(Key, Values);
}

void FNullCleverTapInstance::RemoveValueForKey(const FString& Key)
{
	CleverTapSDK::Ignore(Key);
}

void FNullCleverTapInstance::SetMultiValuesForKey(const FString& Key, const TArray<FString> Values)
{
	CleverTapSDK::Ignore(Key, Values);
}

ECleverTapPushPermissionStatus FNullCleverTapInstance::GetPushPermissionStatus()
{
	return ECleverTapPushPermissionStatus::NotGranted;
}

void FNullCleverTapInstance::PromptForPushPermission(bool bFallbackToSettings)
{
	CleverTapSDK::Ignore(bFallbackToSettings);
}

void FNullCleverTapInstance::PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig)
{
	CleverTapSDK::Ignore(PushPrimerAlertConfig);
}

void FNullCleverTapInstance::PromptForPushPermission(
	const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig)
{
	CleverTapSDK::Ignore(PushPrimerHalfInterstitialConfig);
}

void FNullCleverTapInstance::EnableOnPushNotificationClicked() {}

bool FNullCleverTapInstance::LocalizeAndroidNotificationChannel(
	const FString& ChannelID, const FText& ChannelName, const FText& ChannelDescription)
{
	CleverTapSDK::Ignore(ChannelID, ChannelName, ChannelDescription);
	return false;
}

bool FNullCleverTapInstance::LocalizeAndroidNotificationChannelGroup(const FString& GroupID, const FText& GroupName)
{
	CleverTapSDK::Ignore(GroupID, GroupName);
	return false;
}

void FNullCleverTapInstance::RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler)
{
	CleverTapSDK::Ignore(UrlHandler);
}

void FNullCleverTapInstance::RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter)
{
	CleverTapSDK::Ignore(Filter);
}

void FNullCleverTapInstance::SetOptOut(bool bIsOptingOut)
{
	CleverTapSDK::Ignore(bIsOptingOut);
}
