// Copyright CleverTap All Rights Reserved.
#include "NullCleverTapInstance.h"

FString FNullCleverTapInstance::GetCleverTapId() const
{
	return FString{};
}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile) const {}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) const {}

void FNullCleverTapInstance::PushProfile(const FCleverTapProperties& Profile) const {}

void FNullCleverTapInstance::PushEvent(const FString& EventName) const {}

void FNullCleverTapInstance::PushEvent(const FString& EventName, const FCleverTapProperties& Actions) const {}

void FNullCleverTapInstance::PushChargedEvent(
	const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) const
{
}

void FNullCleverTapInstance::DecrementValue(const FString& Key, int Amount) const {}
void FNullCleverTapInstance::DecrementValue(const FString& Key, double Amount) const {}

void FNullCleverTapInstance::IncrementValue(const FString& Key, int Amount) const {}
void FNullCleverTapInstance::IncrementValue(const FString& Key, double Amount) const {}