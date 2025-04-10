// Copyright CleverTap All Rights Reserved.
#include "NullCleverTapInstance.h"

FString FNullCleverTapInstance::GetCleverTapId()
{
	return FString{};
}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile) {}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) {}

void FNullCleverTapInstance::PushProfile(const FCleverTapProperties& Profile) {}

void FNullCleverTapInstance::PushEvent(const FString& EventName) {}

void FNullCleverTapInstance::PushEvent(const FString& EventName, const FCleverTapProperties& Actions) {}

void FNullCleverTapInstance::PushChargedEvent(
	const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items)
{
}

void FNullCleverTapInstance::DecrementValue(const FString& Key, int Amount) {}
void FNullCleverTapInstance::DecrementValue(const FString& Key, double Amount) {}

void FNullCleverTapInstance::IncrementValue(const FString& Key, int Amount) {}
void FNullCleverTapInstance::IncrementValue(const FString& Key, double Amount) {}

bool FNullCleverTapInstance::IsPushPermissionGranted()
{
	return false;
}

void FNullCleverTapInstance::PromptForPushPermission(bool ShowFallbackSettings) {}
